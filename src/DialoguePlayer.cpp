namespace DialoguePlayer {
    internal_function
    void InitializeState(DialoguePlayerState *n_state) {
        *n_state = {};
        n_state->player_sprite.texture = GetImgResource("../res/imgs/dialoguebox.png");
        n_state->player_sprite.h_frames = 3;
        n_state->player_sprite.transform.scale = {4,4};
        n_state->font = TTF_OpenFont("../res/fonts/monogram.ttf", 16);
        n_state->text.widthMax = 296;
        if (n_state->font == NULL) {
            printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        }
        n_state->recorder_icon.texture = GetImgResource("../res/imgs/recordericon.png");
        n_state->recorder_icon.transform.scale = {2,2};
        n_state->recorder_icon.transform.pos = {280*4,64*4};

        n_state->selector_icon.texture = GetImgResource("../res/imgs/selector.png");
        n_state->selector_icon.transform.scale = {3,3};

        n_state->recorder_option_count = 2;
        n_state->recorder_options[0].set_text("I love you");
        n_state->recorder_options[1].set_text("More, please");
        GenerateFancyTextObjectTexture(n_state->recorder_options[0].raw_text, n_state->recorder_options[0].total_characters, n_state->font, &n_state->recorder_options[0]);
        GenerateFancyTextObjectTexture(n_state->recorder_options[1].raw_text, n_state->recorder_options[1].total_characters, n_state->font, &n_state->recorder_options[1]);
        // strcpy(n_state->recorder_options[0],"I love you");
        // strcpy(n_state->recorder_options[1],"More, please");
    }

    internal_function void completeText(DialoguePlayerState &dialogue_state);
    internal_function void nextText(DialoguePlayerState &dialogue_state);

    internal_function void OnSkipPressed(DialoguePlayerState &dialogue_state) {
        if (dialogue_state.isUpdating) {
            completeText(dialogue_state);
        } else {
            nextText(dialogue_state);
        }
    }

    internal_function
    void Update(DialoguePlayerState &dialogue_state, float timestep) {
        vec2i mPos = GetMousePosition();
        IntRect recorder_rect = {(int)dialogue_state.recorder_icon.transform.pos.x,(int)dialogue_state.recorder_icon.transform.pos.y,dialogue_state.recorder_icon.texture->meta.width*2,dialogue_state.recorder_icon.texture->meta.height*2};
        if (recorder_rect.contains(mPos.x,mPos.y)) {
            dialogue_state.recorder_icon.color = {255,200,110,255};
            if (input->mouse_just_pressed) {
                dialogue_state.recorder_icon.visible = false;
                dialogue_state.recorder_menu_open = true;
                dialogue_state.player_sprite.frame = 2;
                dialogue_state.selection = 0;
            }
        } else {
            dialogue_state.recorder_icon.color = {255,255,255,255};
        }

        if (dialogue_state.recorder_menu_open) {
            if (input->just_pressed[SDL_SCANCODE_C]) {
                dialogue_state.recorder_menu_open = false;
                dialogue_state.recorder_icon.visible = true;
                dialogue_state.player_sprite.frame = 0; 
            } else if (input->just_pressed[SDL_SCANCODE_S]) {
                if (dialogue_state.selection == dialogue_state.recorder_option_count-1) {
                    dialogue_state.selection=0;
                } else {
                    dialogue_state.selection++;
                }
            } else if (input->just_pressed[SDL_SCANCODE_W]) {
                if (dialogue_state.selection == 0) {
                    dialogue_state.selection=dialogue_state.recorder_option_count-1;
                } else {
                    dialogue_state.selection=0;
                }
            }
        }

        if (!dialogue_state.isPlaying) {
            return;
        }
        if (input->just_pressed[SDL_SCANCODE_Z]) {
            OnSkipPressed(dialogue_state);
            return;
        } if ((size_t)dialogue_state.text.visible_characters == dialogue_state.text.total_characters) {
            if (input->just_pressed[SDL_SCANCODE_1]) {
                game_state->script_state->choice_stack.push(0);
                ScriptRunQueue(game_state->script_state);
            } if (input->just_pressed[SDL_SCANCODE_2]) {
                game_state->script_state->choice_stack.push(1);
                ScriptRunQueue(game_state->script_state);
            } if (input->just_pressed[SDL_SCANCODE_3]) {
                game_state->script_state->choice_stack.push(2);
                ScriptRunQueue(game_state->script_state);
            }
            return;
        } if (!dialogue_state.isUpdating) {
            return;
        }
        dialogue_state.time_accumulator += timestep;
        if (dialogue_state.time_accumulator > 0.018) {
            dialogue_state.text.visible_characters++;
            // std::string renderable = dialogue_state.text.substr(0, dialogue_state.visible_chars);
            if (dialogue_state.text.texture.texture != nullptr) {
                SDL_DestroyTexture(dialogue_state.text.texture.texture);
            }
            GenerateFancyTextObjectTexture(dialogue_state.text.raw_text, dialogue_state.text.visible_characters, dialogue_state.font, &dialogue_state.text);//GenerateTextTexture(renderable.c_str(), { 255,255,255,255 }, dialogue_state.font, &dialogue_state.textWidth, &dialogue_state.textHeight, dialogue_state.widthMax);
            dialogue_state.time_accumulator = 0;
            if ((size_t)dialogue_state.text.visible_characters >= dialogue_state.text.total_characters) {
                dialogue_state.isUpdating = false;
            }
        }
    }

    internal_function
    void SetText(DialoguePlayerState &dialogue_state, char *n_text) {
        dialogue_state.text.set_text(n_text);
        dialogue_state.text.visible_characters = 0;
        dialogue_state.time_accumulator = 0;
        dialogue_state.isPlaying = true;
        dialogue_state.isUpdating = true;
        dialogue_state.isDisplayingChoices = false;
    }

    internal_function
    void Render(DialoguePlayerState &dialogue_state) {
        if (!dialogue_state.isPlaying) {
            return;
        }
        DrawSprite(&dialogue_state.player_sprite);
        // DrawTexture(dialogue_state.image_res->texture, frame, dest);
        
        if (dialogue_state.text.texture.texture && dialogue_state.text.texture.meta.width && dialogue_state.text.texture.meta.height) {
            SDL_Rect text_dest = { 14*4, 12*4, dialogue_state.text.texture.meta.width*4, dialogue_state.text.texture.meta.height*4};
            SDL_RenderCopy(renderer->sdl_renderer, dialogue_state.text.texture.texture, NULL, &text_dest);
        }
        if (dialogue_state.recorder_menu_open) {
            for (u8 i=0; i<dialogue_state.recorder_option_count; i++) {
                text_object &option = dialogue_state.recorder_options[i];
                SDL_Rect text_dest = { 250*4, 68*4 + i * 52, option.texture.meta.width*3, option.texture.meta.height*3};
                SDL_RenderCopy(renderer->sdl_renderer, option.texture.texture, NULL, &text_dest);

            }
            dialogue_state.selector_icon.transform.pos = {248*4 - 24.f, 71*4.f + dialogue_state.selection*52};
            DrawSprite(&dialogue_state.selector_icon);
        }

        DrawSprite(&dialogue_state.recorder_icon);
    }

    internal_function
    void completeText(DialoguePlayerState &dialogue_state) {
        dialogue_state.text.visible_characters = dialogue_state.text.total_characters;
        GenerateFancyTextObjectTexture(dialogue_state.text.raw_text, dialogue_state.text.visible_characters, dialogue_state.font, &dialogue_state.text);//GenerateTextTexture(renderable.c_str(), { 255,255,255,255 }, dialogue_state.font, &dialogue_state.textWidth, &dialogue_state.textHeight, dialogue_state.widthMax);
        dialogue_state.isUpdating = false;
    }


    internal_function
    void nextText(DialoguePlayerState &dialogue_state) {
        dialogue_state.isPlaying = false;
        if (dialogue_state.text.texture.texture) {
            SDL_DestroyTexture(dialogue_state.text.texture.texture);
            dialogue_state.text.texture.texture = nullptr;
        }
        ScriptContinueCallback(game_state->script_state);
    }
}
