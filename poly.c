#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct {
  int x, y;
} point;

#if NOGL

// no GL - render to bitmap?
int run() {
  return 0;
}

#else
#include <pthread.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
static GLFWwindow* window;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS &&
      key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    printf("scancode = %d, mods = %d\n", scancode, mods);
}

void draw_triangle(point pts[3]) {

  glBegin(GL_TRIANGLES);

    glColor4f(0.5, 1, 1, 1);
    glVertex2i(pts[0].x, pts[0].y);
    glVertex2i(pts[1].x, pts[1].y);
    glVertex2i(pts[2].x, pts[2].y);

  glEnd();
}

void draw_line_strip(int n, point *pts, int wrap) {

  glBegin(GL_LINE_STRIP);
  glColor4f(1, 1, 1, 1);

  for (int i=0; i<n; i++)
    glVertex2i(pts[i].x, pts[i].y);

  if (wrap)
    glVertex2i(pts[0].x, pts[0].y);

  glEnd();
}

int run() {

  const GLubyte *renderer;
  const GLubyte *version;

  int gl_width = 320;
  int gl_height = 200;

  if (!glfwInit()) {
    fprintf(stderr,
      "couldn't initialize glfw3\n");
    return -1;
  }

  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(gl_width, gl_height, "Polygon test", NULL, NULL);

  if (!window) {
    fprintf(stderr,
      "couldn't initialize GLFWwindow\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  renderer = glGetString(GL_RENDERER);
  version = glGetString(GL_VERSION);

  fprintf(stdout,
    "GL_RENDERER: %s\n"
    "GL_VERSION: %s\n",
    renderer, version);

  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  glEnable(GL_BLEND);

  while (!glfwWindowShouldClose(window)) {

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fb_width, fb_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, gl_width, 0.f, gl_height, 0.f, 1.f);

    point ctr = {gl_width/2, gl_height/2};

    point vertices[] = {
      {ctr.x,     ctr.y   },
      {ctr.x+32,  ctr.y-32},
      {ctr.x,     ctr.y-32},

      {ctr.x,     ctr.y   },
      {ctr.x-32,  ctr.y+32},
      {ctr.x,     ctr.y+32}
    };

    draw_line_strip(6, vertices, 1);
    point triangle0[3] = { {10, 10}, {40, 10}, {10, 40} };
    draw_triangle(triangle0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

#endif

int main(int argc, char **argv) {

  int h = 200, w = 320;
  int buf_size = h * w * sizeof(u8);

  u8 *buf = malloc(buf_size);

  printf("buf: w=%d x h=%d (%d B), ptr=%p\n", w, h, buf_size, buf);

  int res = run();
  printf("res = %d\n", res);
  if (buf) free(buf);
  return 0;
}
