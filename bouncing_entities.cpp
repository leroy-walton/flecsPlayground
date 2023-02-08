
#include <raylib.h>
#include <flecs.h>
#include <iostream>
#include <cmath>

struct Position
{
    double x, y;
};

struct Velocity
{
    double x, y;
};

struct Attractor
{
    double strength;
};


float distance(Position p1, Position p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}



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
    
    // System that simulates attraction between all entities
    //     for (flecs::entity e : it) {
    //         auto &p = e.get<Position>();
    //         for (auto e2 : it) {
    //             if (e != e2) {
    //                 auto &p2 = e2.get<Position>();
    //                 auto dx = p2.x - p.x;
    //                 auto dy = p2.y - p.y;
    //                 auto distance = sqrt(dx * dx + dy * dy);
    //                 if (distance > 0) {
    //                     auto force = 1 / distance;
    //                     e.modify<Velocity>([&](Velocity &v) {
    //                         v.x += dx * force;
    //                         v.y += dy * force;
    //                     });
    //                 }
    //             }
    //         }
    //     }
    // });
        //     auto &p2 = e.get<Position>();
        //     auto &v2 = e.get<Velocity>();
        //     auto d = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        //     auto f = 1.0 / (d * d);
        //     v1.x += (p2.x - p1.x) * f;
        //     v1.y += (p2.y - p1.y) * f;
    
    InitWindow(screenWidth, screenHeight, "Flecs - Raylib Example");
    SetTargetFPS(60);

    // Create a few test entities 
    const int numEntities = 500;
    for (int i = 0; i < numEntities; i++)
    {
        auto e = ecs.entity();
        e.set<Position>({(float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight)});
        e.set<Velocity>({(float)GetRandomValue(-5, 5), (float)GetRandomValue(-5, 5)});
    }

    while (!WindowShouldClose())
    {
        ecs.progress();

        BeginDrawing();
        ClearBackground(BLACK);

        ecs.query<Position>().each([](flecs::entity e, Position &p)
                                   { DrawCircle(p.x, p.y, 2, (Color){255, 121, 241, 255}); });

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}