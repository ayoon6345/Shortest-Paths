#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <map>
#include <queue>
#include "graphics.h"
#include <math.h>
using namespace std;

/* Image code borrowed from our USA map demo... */
typedef pair<int, int> Node;
struct Pixel
{
  unsigned char r, g, b;
};
bool operator==(Pixel &a, Pixel &b)
{
  return a.r == b.r && a.g == b.g && a.b == b.b;
}
bool operator!=(Pixel &a, Pixel &b)
{
  return !(a == b);
}
int width, height;
Pixel *image;
int adjust;
Pixel white = {255, 255, 255};
Pixel black = {0, 0, 0};
Pixel red = {255, 0, 0};

vector<Node> all_nodes;
queue<Node> to_visit;
map<Node, vector<Node>> nbrs;
map<Node, vector<Node>> seamNbrs;
map<Node, int> dist;
map<int, unsigned char> dim;
map<Node, int> weight;
map<Node, Node> pred;

Pixel &get_pixel(int x, int y)
{
  return image[y * width + x];
}

void read_image(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  int dummy = fscanf(fp, "P6\n%d %d\n255%*c", &width, &height);
  image = new Pixel[width * height];
  dummy = fread(image, width * height, sizeof(Pixel), fp);
  fclose(fp);
}

// Import this from your solution to the first part...

//Build graph for breath first search blurring
Node meganode;
void calculate_transport(void)
{
  bool mNode = false;

  int di[] = {+1, -1, 0, 0};
  int dj[] = {0, 0, +1, -1};
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      all_nodes.push_back(make_pair(i, j));
      if (get_pixel(i, j) == white)
      {
        if (mNode == false)
        {
          meganode = make_pair(i, j);
          mNode = true;
        }
        Node nbr = make_pair(i, j);
        nbrs[meganode].push_back(nbr);
      }
      for (int k = 0; k < 4; k++)
      {

        Node x = make_pair(i, j);
        Node nbr = make_pair(i + di[k], j + dj[k]);
        nbrs[x].push_back(nbr);
      }
    }
  }
}
//breath first search for blur
void calculate_blur(void)
{
  for (Node &a : all_nodes)
    dist[a] = all_nodes.size();
  dist[meganode] = 0;
  to_visit.push(meganode);
  while (!to_visit.empty())
  {
    Node x = to_visit.front();
    to_visit.pop();
    for (Node n : nbrs[x])
      if (dist[n] == all_nodes.size())
      {
        dist[n] = 1 + dist[x];
        if (get_pixel(n.first, n.second) == black)
        {
          Pixel p = {(unsigned char)(255 * pow(0.9, dist[n])), (unsigned char)(255 * pow(0.9, dist[n])), (unsigned char)(255 * pow(0.9, dist[n]))};
          get_pixel(n.first, n.second) = p;
        }
        to_visit.push(n);
      }
  }
}

// To be written -- solve a shortest path problem to find a seam and color it red

//builds graph for seams
Node megaseamnode;
void build_seamgraph(void)
{
  bool msNode = false;
  int di[] = {-1, 0, +1};
  int dj[] = {+1, +1, +1};
  for (int i = 1; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      if (j == 0)
      {
        if (msNode == false)
        {
          megaseamnode = make_pair(i, j);
          msNode = true;
        }
        Node nbr = make_pair(i, j);
        seamNbrs[megaseamnode].push_back(nbr);
      }
      for (int k = 0; k < 3; k++)
      {
        Node x = make_pair(i, j);
        Node nbr = make_pair(i + di[k], j + dj[k]);
        if ((x.second != height - 1) && (nbr.first >= 0) && (nbr.first < width - 2))
        {
          seamNbrs[x].push_back(nbr);
        }
      }
    }
  }
}
//follow predecessor and mark pixels red
void mark_path(Node y)
{
  if (y.second != 0)
    mark_path(pred[y]);
  get_pixel(y.first, y.second) = red;
}
//Calculate seam using Dijkstra algorithm
Node ending;
void calculate_seam(void)
{
  build_seamgraph();
  for (Node &a : all_nodes)
    weight[a] = all_nodes.size();
  weight[megaseamnode] = 0;
  queue<Node> seam_visit;
  seam_visit.push(megaseamnode);
  while (!seam_visit.empty())
  {
    Node x = seam_visit.front();
    seam_visit.pop();
    for (Node n : seamNbrs[x])
      if (weight[x] + (int)get_pixel(n.first, n.second).r < weight[n])
      {
        weight[n] = weight[x] + (int)get_pixel(n.first, n.second).r;
        pred[n] = x;
        seam_visit.push(n);
      }
    if (x.second == 399)
    {
      ending = make_pair(x.first, x.second);
    }
  }
  mark_path(ending);
  seamNbrs.clear();
  pred.clear();
  weight.clear();
}

/* Code to find and remove seams */

bool seam_exists(void)
{
  for (int x = 0; x < width; x++)
    if (get_pixel(x, 0) == red)
      return true;
  return false;
}

void remove_seam(void)
{
  cout << "Removing seam to decrease width to " << width - 1 << "\n";
  for (int y = 0; y < height; y++)
  {
    int where_red = -1;
    for (int x = 0; x < width; x++)
      if (get_pixel(x, y) == red)
        if (where_red != -1)
        {
          cout << "Error: row " << y << " hass >1 red pixel set\n";
          exit(0);
        }
        else
          where_red = x;
    if (where_red == -1)
    {
      cout << "Error: row " << y << " has 0 red pixels set\n";
      exit(0);
    }
    for (int x = where_red; x < width - 1; x++)
      get_pixel(x, y) = get_pixel(x + 1, y);
  }

  // Create a new image with one smaller width
  Pixel *orig_image = image;
  width--;
  image = new Pixel[height * width];
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
      get_pixel(x, y) = orig_image[y * (width + 1) + x];
  delete[] orig_image;

  calculate_blur();
}

/* Simple 2D interactive graphics code... */

bool show_blurred_image = false;

// Called on each keypress
void keyhandler(int key)
{
  if (key == 'q')
    exit(0);

  // Toggle showing blurred image (off-white pixels)
  if (key == 'b')
    show_blurred_image = !show_blurred_image;

  // Find and remove one seam
  if (key == 's' && !seam_exists())
    calculate_seam();

  // Find and remove 2...9 seams
  if ((key >= '2' && key <= '9') && !seam_exists())
    for (int i = 0; i < key - '0'; i++)
    {
      calculate_seam();
      remove_seam();
    }
}

// Called any time the scene needs to be re-rendered
void render(void)
{
  // Always plot white and red pixels.  Show all other pixels if blur
  // mode is toggled on
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      Pixel p = get_pixel(x, y);
      if (show_blurred_image || p == white || p == red)
      {
        set_color(p.r, p.g, p.b);
        draw_pixel(x, y);
      }
    }

  // Fill in empty space at right from shrinking the image with blue
  set_color(0, 0, 255);
  for (int x = width; x < 800; x++)
    draw_line(x, 0, x, height - 1);
}

// Called periodically (msec is amount of time since last called)
// Return true if a re-render is needed
// Right now this function waits a second after finding a seam to
// remove it (allowing it to be visualized briefly)
bool timer(int msec)
{
  static int cumulative = 0;
  static bool found_seam = false;
  cumulative += msec;
  if (cumulative > 1000)
  {
    cumulative -= 1000;
    if (seam_exists())
    {
      if (found_seam)
      {
        remove_seam();
        found_seam = false;
        return true;
      }
      found_seam = true;
    }
  }
  return false;
}

int main(int argc, char *argv[])
{
  read_image("billboard.ppm");
  calculate_transport();

  calculate_blur();
  init_graphics(argc, argv, width, height, render, keyhandler, timer);
}
