
#include "Shader.cpp"

const Color COLOR_BLACK = Color(0,0,0,255);
const Color COLOR_WHITE = Color(255,255,255,255);
const Color COLOR_RED = Color(255,0,0,255);
const Color COLOR_BLUE = Color(0,0,255,255);
const Color COLOR_GREEN = Color(0,255,0,255);
const Color COLOR_YELLOW = Color(255,255,0,255);
const Color COLOR_TRANSPARENT = Color(0,0,0,0);


v2i global_draw_offset={0,0};

void GL_PushOffset(v2i offset) {
    global_draw_offset = offset;
}

void GL_PopOffset() {
    global_draw_offset={0,0};
}



void GL_DrawRect(iRect rect) {
    rect.x += global_draw_offset.x;
    rect.y += global_draw_offset.y;

    float vertices[] = {
        (float)rect.x+rect.w,    (float)rect.y,           0.0f,
        (float)rect.x+rect.w,    (float)rect.y+rect.h,    0.0f,
        (float)rect.x,           (float)rect.y+rect.h,    0.0f,
        (float)rect.x,           (float)rect.y,           0.0f
    };

    // we don't have to repass the projection every frame, unless it changes
    local_persist GLuint VAO,VBO;
    local_persist bool generated=false;
    if (!generated) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        generated=true;
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);    
}

void GL_DrawTexture(iRect source, iRect dest, bool flip_x=false, bool flip_y=false) {
    float u1 = 0.0f;
    float u2 = 1.0f;
    float v1 = 0.0f;
    float v2 = 1.0f;

    dest.x += global_draw_offset.x;
    dest.y += global_draw_offset.y;

    v2i tex_size = {0,0};
    if (source.w!=0) {
        // store this info in some data structure maybe?
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_size.x);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_size.y);
        if (source.w!=0) {
            (flip_x ? u2 : u1) = (float)source.x / (float)tex_size.x;
            (flip_x ? u1 : u2) = (float)(source.x + source.w) / (float)tex_size.x;
            (flip_y ? v2 : v1) = (float)source.y / (float)tex_size.y;
            (flip_y ? v1 : v2) = (float)(source.y + source.h) / (float)tex_size.y;
        }
    }
    
    float vertices[] = {
        (float)dest.x,           (float)dest.y,        0.0f, u1, v1,
        (float)dest.x+dest.w,    (float)dest.y,        0.0f, u2, v1,
        (float)dest.x+dest.w,    (float)dest.y+dest.h, 0.0f, u2, v2,
        (float)dest.x,           (float)dest.y+dest.h, 0.0f, u1, v2,
    };
    
    // uniforms
    local_persist GLuint VAO,VBO;
    local_persist bool generated=false;
    if (!generated) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        generated=true;
    }
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);    
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

/*
void GL_DrawTextureEx(iRect source={0,0,0,0}, iRect dest={0,0,0,0}, bool flip_x=false, bool flip_y=false, float rotation=0) {
    float u1= !flip_x ? 0.0f : 1.0f;
    float u2= !flip_x ? 1.0f : 0.0f;
    float v1= !flip_y ? 0.0f : 1.0f;
    float v2= !flip_y ? 1.0f : 0.0f;

    v2i tex_size = {0,0};
    if (dest.w==0 || src.w!=0) {
        // store this info in some data structure maybe?
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_size.x);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_size.y);
        if (src.w!=0) {
            (flip_x ? u2 : u1) = (float)src.x / (float)tex_size.x;
            (flip_x ? u1 : u2) = (float)(src.x + src.w) / (float)tex_size.x;
            (flip_y ? v2 : v1) = (float)src.y / (float)tex_size.y;
            (flip_y ? v1 : v2) = (float)(src.y + src.h) / (float)tex_size.y;
        }
    } if (dest.w==0) {
        dest.w = tex_size.x;
        dest.h = tex_size.y;
    }
    
    float vertices[] = {
        (float)dest.x,           (float)dest.y,        0.0f, u1, v1,
        (float)dest.x+dest.w,    (float)dest.y,        0.0f, u2, v1,
        (float)dest.x+dest.w,    (float)dest.y+dest.h, 0.0f, u2, v2,
        (float)dest.x,           (float)dest.y+dest.h, 0.0f, u1, v2,
    };
        
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbuffers[TEXTURE_VBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // uniforms
    GLint textureLoc = glGetUniformLocation(sh_textureProgram,"_texture");
    glUniform1i(textureLoc,0);
    
    glm::mat4 model = glm::mat4(1.0f);
    if (rotation != 0) {
        glm::vec2 pos = glm::vec2(dest.x, dest.y);
        float angleRadians = rotation;
        // Calculate the center of the object for rotation
        glm::vec2 size = glm::vec2(dest.w,dest.h);
        glm::vec2 center = pos + size * 0.5f;
        model = glm::translate(model, glm::vec3(center, 0.0f)); // Move pivot to center
        model = glm::rotate(model, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
        model = glm::translate(model, glm::vec3(-center, 0.0f)); // Move pivot back
    }
    
    GLint transformLoc = glGetUniformLocation(sh_textureProgram,"model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(gl_varrays[TEXTURE_VAO]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
}
*/

glm::mat4 rotate_model_matrix(float angleRadians, iRect rect, v2 origin) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec2 pos = glm::vec2(rect.x, rect.y);
    // Calculate the center of the object for rotation
    glm::vec2 center = pos + glm::vec2(origin.x,origin.y);
    model = glm::translate(model, glm::vec3(center, 0.0f)); // Move pivot to center
    model = glm::rotate(model, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
    model = glm::translate(model, glm::vec3(-center, 0.0f)); // Move pivot back
    return model;
}


glm::mat4 rotate_model_matrix(float angleRadians, iRect rect) {
    v2 origin = {rect.w/2.f,rect.h/2.f};
    return rotate_model_matrix(angleRadians,rect,origin);
}


internal i32 GL_load_texture(GLuint tex, const char* path) {
    SDL_Surface *tex_surf = IMG_Load(path);
    if (tex_surf == NULL) {
        std::cout << "ERROR: Failed to load texture at path " << path << std::endl;
        return 0;
    }

    int Mode = GL_RGB;
    if(tex_surf->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_surf->w, tex_surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return 1;
}

internal void GL_load_texture_from_sdl_surface(GLuint tex,SDL_Surface *tex_surf) {
    GLenum format = 0, type = GL_UNSIGNED_BYTE; // Default to unsigned byte for type

    switch (tex_surf->format->format) {
        case SDL_PIXELFORMAT_RGBA8888:
            format = GL_RGBA;
            break;
        case SDL_PIXELFORMAT_ABGR8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_BGRA8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_ARGB8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_RGB888:
        case SDL_PIXELFORMAT_RGB24:
            format = GL_RGB;
            break;
        case SDL_PIXELFORMAT_BGR888:
            format = GL_BGR;
            break;
        case SDL_PIXELFORMAT_RGB565:
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case SDL_PIXELFORMAT_RGBA4444:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case SDL_PIXELFORMAT_RGB332:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE_3_3_2;
            break;
        default:
            break;
    }

    if (format == 0) {
        std::cerr << "No valid OpenGL format found. Texture creation aborted." << std::endl;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format == GL_RGB ? GL_RGB8 : GL_RGBA8, tex_surf->w, tex_surf->h, 0, format, type, tex_surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
}




internal void generate_text(GLuint tex, TTF_Font *font,std::string str,Color col,i32 wrap) {
    SDL_Surface* temp_surface =
        TTF_RenderText_Solid_Wrapped(font, str.c_str(),*(SDL_Color*)&col, wrap);
    temp_surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_ARGB8888, 0);

    if (tex != NULL) {
        glDeleteTextures(1,&tex);
    }
    glGenTextures(1,&tex);

    GL_load_texture_from_sdl_surface(tex,temp_surface);
    
    SDL_FreeSurface(temp_surface);
}

struct generic_drawable {
    v2i position={0,0};
    GLuint gl_texture=NULL;
    v2 scale={1,1};
    iRect bound = {0,0,0,0};

    iRect get_draw_rect() {
        if (bound.w==0||bound.h==0) {
            glBindTexture(GL_TEXTURE_2D,gl_texture);
            // mipmap level is 0
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &bound.w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &bound.h);
        }
        int w=bound.w,h=bound.h;
        iRect res={position.x,position.y,(int)((float)w*scale.x),(int)((float)h*scale.y)};
        return res;
    }
};


internal generic_drawable generate_text_obj(TTF_Font *font,std::string str,Color col={255,255,255,255},GLuint tex=NULL) {
    generic_drawable res;
    SDL_Surface* temp_surface =
        TTF_RenderText_Solid(font, str.c_str(),*(SDL_Color*)&col);
    temp_surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_ARGB8888, 0);

    if (tex == NULL) {
        glGenTextures(1,&tex);
    }

    res.gl_texture = tex;
    GL_load_texture_from_sdl_surface(res.gl_texture,temp_surface);
    
    SDL_FreeSurface(temp_surface);
    return res;
}


struct camera_t {
    v2 pos={0,0};
    v2 size;
    float zoom=1.0f;
     
    iRect get_draw_rect() {
        return {(int)(pos.x-size.x/2),(int)(pos.y-size.y/2),(int)size.x,(int)size.y};
    }

    v2 get_offset() {
        return pos - (v2(size.x/2.f,size.y/2.f));
    }

    v2 get_center() {
        return pos + (v2(size.x/2.f,size.y/2.f));
    }
};



Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

internal
GLuint GL_create_framebuffer(GLuint texture) {
    GLuint fb;
    glGenFramebuffers(1,&fb);
    glBindFramebuffer(GL_FRAMEBUFFER,fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return fb;
}

internal
void GL_load_texture_for_framebuffer(GLuint texture,i32 width,i32 height) {
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);
}
