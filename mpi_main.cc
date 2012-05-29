#include <iostream>
using namespace std;

#include "mpi.h"

#include "base/Ray.h"
#include "scene/Camera.h"
#include "scene/Scene.h"
#include "materials/Material.h"
#include "pic/PPMFile.h"
#include "engine/PathTracingEngine.h"
#include "engine/RayTracingEngine.h"
#include "engine/AntiAliasingEngine.h"

typedef struct {
  int start;
  int size;
} MESSAGE;

void render(Color *out, int start, int n,
    int size, Scene &scene, Camera &camera, int samples,
    AntiAliasingEngine &aae, int super_sample_num, double sp_len) {
  for (int i = 0; i < n; ++i) {
    int c = start + i;
    int y = c / size;
    int x = c % size;
    double sy = 1 - static_cast<double>(y) / size;
    double sx = static_cast<double>(x) / size;
    Color color = Color::kBlack;
    if(aae.ShouldSuperSample(x, y)) {
      Color sp_color = Color::kBlack;
      double spx,spy;
      for (int k = -super_sample_num; k < super_sample_num + 1; ++k) {
        spy = sy + k * sp_len;
        for (int l = -super_sample_num; l < super_sample_num + 1; ++l) {
          spx = sx + l * sp_len;
          Ray ray = camera.GenerateRay(spx, spy);
          for(int j = 0; j < samples; j++) {
            sp_color = sp_color.Add(PathTracingEngine::PathTracing(scene, ray, 0, 1));
          }
          sp_color = sp_color.Multiply(1.0 / samples);
          color = color.Add(sp_color);
        }
      }
      out[i] = color.Multiply(1.0 / ((2 * super_sample_num + 1) * (2 * super_sample_num + 1)));
    } else {
      Ray ray = camera.GenerateRay(sx, sy);
      for(int j = 0; j < samples; j++) {
        color = color.Add(PathTracingEngine::PathTracing(scene, ray, 0, 1));
      }
      out[i] = color.Multiply(1.0/samples);
    }
  }
}

void proc0(int numprocs, int size,
    Scene &scene, Camera &camera, int samples,
    AntiAliasingEngine &aae, int super_sample_num, double sp_len) {
  int num_pix = size * size;
  Color *color_arr = new Color [num_pix];
  int q = num_pix / numprocs;
  int r = num_pix % numprocs;
  MESSAGE msg;
  int start = 0;
  for (int i = 1; i < numprocs; ++i) {
    int n = (i <= r) ? (q+1) : q;
    msg.start = start;
    msg.size = n;
    MPI_Send(&msg, sizeof(msg), MPI_BYTE, i, 99, MPI_COMM_WORLD);
    start += n;
  }
  render(color_arr+start, start, q,
      size, scene, camera, samples,
      aae, super_sample_num, sp_len);
  start = 0;
  MPI_Status status;
  for (int i = 1; i < numprocs; ++i) {
    int n = (i <= r) ? (q+1) : q;
    MPI_Recv(color_arr+start, sizeof(Color)*n, MPI_BYTE, i, 99, MPI_COMM_WORLD, &status);
    start += n;
  }
  PPMFile ppmfile(size, size);
  ppmfile.DataCopy(color_arr, 0, num_pix);
  ppmfile.Save("image.ppm");
  delete [] color_arr;
}

void procx(int rank, int size,
    Scene &scene, Camera &camera, int samples,
    AntiAliasingEngine &aae, int super_sample_num, double sp_len) {
  MESSAGE msg;
  MPI_Status status;
  MPI_Recv(&msg, sizeof(msg), MPI_BYTE, 0, 99, MPI_COMM_WORLD, &status);
  Color *out = new Color[msg.size];
  render(out, msg.start, msg.size,
      size, scene, camera, samples,
      aae, super_sample_num, sp_len);
  MPI_Send(out, sizeof(Color)*msg.size, MPI_BYTE, 0, 99, MPI_COMM_WORLD);
  delete [] out;
}

int main(int argc, char **argv) {
  int rank, numprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  cout << "start" << endl;
  double distance = 5.0;
  Camera camera(0.0,0.0,distance, 0.0,1.0,0.0, 0.0,0.0,-1.0, -distance, 45);
  Scene scene;
  Material source(Color(1,0,0), Color(), Color(), Color::kWhite.Multiply(1));
  Material source2(Color(1,0,0), Color(), Color(), Color::kWhite.Multiply(3));
  Material diffuse_green(Color(0,1,0), Color());
  Material diffuse_white(Color::kWhite, Color());
  Material diffuse_red(Color(1,0,0), Color());
  Material diffuse_blue(Color(0,0,1), Color());
  Material reflect(Color::kWhite.Multiply(0.9), Color::kWhite.Multiply(0.1));
  Material refract(Color::kGray(0.1), Color::kGray(0), Color::kGray(0.9), 1);
  scene.CreatePlane(0, 1, 0, 2, source);
  scene.CreateSphere(Vec(-1,-0.5,0), 0.5, diffuse_green);
  scene.CreateSphere(Vec(0,-0.5,0), 0.5, source2);
  scene.CreateSphere(Vec(1,-0.5,0), 0.5, diffuse_red);
//  scene.CreateSphere(Vec(0,-1,0.5), 1, refract);
  scene.CreatePlane(0, 1, 0, -1, diffuse_white);
  scene.CreateTriangle(Vec(0, 0, 0), Vec(-1, 1, -1), Vec(1, 1, -1), diffuse_blue);

  //scene.CreateSphere(Vec(-1,-1,0), 0.5, reflect);
//  scene.CreateSphere(Vec(2, 2, 2), 2, material);
//  scene.CreateSphere(Vec(-2, -2, 0), 3, material);
  int size = 256;
  int samples = 64;
  AntiAliasingEngine aae = AntiAliasingEngine(size, size);
  aae.LoadScene(scene, camera);
  int super_sample_num = 2;
  double sp_len = 1.0 / (size * 2 * super_sample_num);
  if (rank == 0) {
    proc0(numprocs,
        size, scene, camera, samples,
        aae, super_sample_num, sp_len);
  } else {
    procx(rank,
        size, scene, camera, samples,
        aae, super_sample_num, sp_len);
  }
  MPI_Finalize();
  return 0;
}
