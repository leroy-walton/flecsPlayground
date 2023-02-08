
#include <raylib.h>
#include <flecs.h>
#include <iostream>
#include <cmath>

#define NUM_ENTITIES 10
#define GRAVITY 5.0f

struct Position
{
    double x, y;
};

struct Velocity
{
    double x, y;
};

int main(int, char *[])
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;

    flecs::world ecs;

    // Add Position and Velocity components
    ecs.component<Position>();
    ecs.component<Velocity>();

    // System that moves entities based on their velocity.
    ecs.system<Position, Velocity>("sysMove")
        .each([](Position &p, Velocity &v)
              {
                  p.x += v.x;
                  p.y += v.y; });

    ecs.system<Position, Velocity>("sysEdgesCollisions")
        .each([](Position &p, Velocity &v)
              {
                if ( p.x < 0 ) v.x = -v.x;            
                if ( p.y < 0 ) v.y = -v.y;            
                if ( p.x > screenWidth ) v.x = -v.x;
                if ( p.y > screenHeight ) v.y = -v.y; });

    ecs.system<Position, Velocity>("sysGravity")
        .each([&ecs](flecs::iter &it, size_t index, Position &p, Velocity &v)
              {
                auto e1 = it.entity(index);

                for( auto i : it) {
                    auto e2 = it.entity(i);
                    if ( e1 != e2 ) {
                        const auto &p2 = e2.get<Position>();
                        
                        // Calculate the distance between the two entities
                        float distance = sqrt(( p.x - p2->x ) * ( p.x - p2->x) +
                                      (p.y - p2->y) * (p.y - p2->y));

                        // Apply gravitational force
                        float force = GRAVITY / (distance * distance);
                        v.x += force * (p2->x - p.x) ;
                        v.y += force * (p2->y - p.y) ; 
                    }
                } });

    InitWindow(screenWidth, screenHeight, "Flecs - Raylib Example");
    SetTargetFPS(60);

    const int numEntities = NUM_ENTITIES;
    for (int i = 0; i < numEntities; i++)
    {
        auto e = ecs.entity();
        e.set<Position>({(float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight)});
        e.set<Velocity>({(float)GetRandomValue(-3, 3), (float)GetRandomValue(-3, 3)});
    }

    while (!WindowShouldClose())
    {
        ecs.progress();
        BeginDrawing();
        ClearBackground(BLACK);

        ecs.query<Position>().each([](flecs::entity e, Position &p)
                                   { DrawCircle(p.x, p.y, 3, (Color){255, 121, 241, 255}); });

        DrawFPS(10, 10);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}