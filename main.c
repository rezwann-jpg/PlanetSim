#include "include/SFML/Graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_TITLE "PlanetSim"

#define WINDOW_HIGHT 600
#define WINDOW_WIDTH 800
#define PLANET_COUNT 2
#define G 6.67430e-11

#define TARGET_FPS 60
#define FRAME_DURATION (1.0f / TARGET_FPS)
#define TIME_SCALE 0.01f

#define TRAIL_POINTS 200

typedef struct {
    sfCircleShape* shape;
    sfVector2f position;
    sfVector2f velocity;
    float mass;
    sfVertexArray *trail;
    sfVector2f trailPoints[TRAIL_POINTS];
    int trailIndex;
} Planet;

void initializePlanets(Planet planets[]) {
    planets[0].mass = 5.972e20;
    planets[0].position = (sfVector2f){ WINDOW_WIDTH / 2, WINDOW_HIGHT / 2 };
    planets[0].velocity = (sfVector2f){ 0, 0 };
    planets[0].shape = sfCircleShape_create();
    sfCircleShape_setRadius(planets[0].shape, 20);
    sfCircleShape_setFillColor(planets[0].shape, sfYellow);
    sfCircleShape_setOrigin(planets[0].shape, (sfVector2f){ 20, 20 });
    sfCircleShape_setPosition(planets[0].shape, planets[0].position);
    planets[0].trail = sfVertexArray_create();
    sfVertexArray_setPrimitiveType(planets[0].trail, sfLineStrip);
    planets[0].trailIndex = 0;

    planets[1].mass = 7.348e18;
    planets[1].position = (sfVector2f){ WINDOW_WIDTH / 2 + 200, WINDOW_HIGHT / 2 };
    planets[1].velocity = (sfVector2f){ 0, -10000 };
    planets[1].shape = sfCircleShape_create();
    sfCircleShape_setRadius(planets[1].shape, 10);
    sfCircleShape_setFillColor(planets[1].shape, sfGreen);
    sfCircleShape_setOrigin(planets[1].shape, (sfVector2f){ 10, 10 });
    sfCircleShape_setPosition(planets[1].shape, planets[1].position);
    planets[1].trail = sfVertexArray_create();
    sfVertexArray_setPrimitiveType(planets[1].trail, sfLineStrip);
    planets[1].trailIndex = 0;
}

sfVector2f calculateGravitationalForce(Planet *p1, Planet *p2) {
    sfVector2f force;
    float dx = p2->position.x - p1->position.x;
    float dy = p2->position.y - p1->position.y;
    float dist = sqrt(dx * dx + dy * dy);
    float forceMagnitude = ((G * p1->mass * p2->mass) / (dist * dist));
    force.x = forceMagnitude * (dx / dist);
    force.y = forceMagnitude * (dy / dist);
    return force;
}

void updatePlanets(Planet planets[], float dt) {
    for (int i = 0; i < PLANET_COUNT; i++) {
        sfVector2f totalForce = { 0, 0 };
        for (int j = 0; j < PLANET_COUNT; j++) {
            if (i != j) {
                sfVector2f force = calculateGravitationalForce(&planets[i], &planets[j]);
                totalForce.x += force.x;
                totalForce.y += force.y;
            }
        }

        planets[i].velocity.x += (totalForce.x / planets[i].mass) * dt;
        planets[i].velocity.y += (totalForce.y / planets[i].mass) * dt;
        planets[i].position.x += planets[i].velocity.x * dt;
        planets[i].position.y += planets[i].velocity.y * dt;
        sfCircleShape_setPosition(planets[i].shape, planets[i].position);

        planets[i].trailPoints[planets[i].trailIndex] = planets[i].position;
        planets[i].trailIndex = (planets[i].trailIndex + 1) % TRAIL_POINTS;
        
        sfVertexArray_clear(planets[i].trail);
        for (int k = 0; k < TRAIL_POINTS; k++) {
            int index = (planets[i].trailIndex + k) % TRAIL_POINTS;
            sfVertex vertex = { .position = planets[i].trailPoints[index], .color = sfColor_fromRGBA(103, 242, 209, 128) };
            sfVertexArray_append(planets[i].trail, vertex);
        }
    }
}

int main(void) {
    sfRenderWindow *window;
    sfVideoMode mode;
    mode.width = WINDOW_WIDTH;
    mode.height = WINDOW_HIGHT;
    mode.bitsPerPixel = 32;

    window = sfRenderWindow_create(mode, WINDOW_TITLE, sfResize | sfClose, NULL);
    if(!window) {
        return EXIT_FAILURE;
    }

    Planet planets[PLANET_COUNT];
    initializePlanets(planets);

    sfClock *clock = sfClock_create();
    sfClock *frameClock = sfClock_create();

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
            }
        }

        float dt = sfTime_asSeconds(sfClock_restart(clock)) * TIME_SCALE;

        sfRenderWindow_clear(window, sfBlack);
        for (int i = 0; i < PLANET_COUNT; i++) {
            sfRenderWindow_drawVertexArray(window, planets[i].trail, NULL);
            sfRenderWindow_drawCircleShape(window, planets[i].shape, NULL);
        }
        sfRenderWindow_display(window);

        updatePlanets(planets, dt);

        // sfSleep(sfSeconds(5));

        float frameTime = sfTime_asSeconds(sfClock_getElapsedTime(frameClock));
        if (frameTime < FRAME_DURATION) {
            sfSleep(sfSeconds(FRAME_DURATION - frameTime));
        }
        sfClock_restart(frameClock);
    }

    for (int i = 0; i < PLANET_COUNT; i++) {
        sfCircleShape_destroy(planets[i].shape);
        sfVertexArray_destroy(planets[i].trail);
    }

    sfClock_destroy(clock);
    sfClock_destroy(frameClock);
    sfRenderWindow_destroy(window);
    return 0;
}