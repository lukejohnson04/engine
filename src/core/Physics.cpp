internal_function inline
bool AABBIsColliding(FloatRect &a, FloatRect &b)
{
    if (a.w == 0 || a.h == 0 || b.w == 0 || b.h == 0)
    {
        return false;
    }
    bool res = a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
    return res;
}

internal_function inline
bool EntitiesAreColliding(Entity *a, Entity *b)
{
    FloatRect l_rect = a->collider;
    l_rect.x += a->transform.pos.x;
    l_rect.y += a->transform.pos.y;

    FloatRect r_rect = b->collider;
    r_rect.x += b->transform.pos.x;
    r_rect.y += b->transform.pos.y;
    return AABBIsColliding(l_rect, r_rect);
}
