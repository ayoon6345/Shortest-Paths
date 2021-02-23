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
int width, height;
Pixel *image;

Pixel white = {255, 255, 255};
Pixel black = {0, 0, 0};

typedef pair<int, int> Node;
vector<Node> all_nodes;
queue<Node> to_visit;
map<Node, vector<Node> > nbrs;
map<Node, int> dist;
map<int, unsigned char> dim;
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

void write_image(const char *filename)
{
  FILE *fp = fopen(filename, "w");
  fprintf(fp, "P6\n%d %d\n255\n", width, height);
  int dummy = fwrite(image, width * height, sizeof(Pixel), fp);
  fclose(fp);
}

/* To be completed from here on... */

//Builds graph for neighboring pixels
Node meganode;
void calculate_transport(void)
{
  bool mNode = false;

  int di[] = {+1, -1, 0, 0};
  int dj[] = {0, 0, +1, -1};
  for (int i = 1; i < height - 1; i++)
  {
    for (int j = 1; j < width - 1; j++)
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

//uses breath first search algorithm to blur picture
vector<double> d;
void calculate_blur(void)
{
  for(int i=0; i<60;i++){
    d.push_back(255 * pow(0.9, i));
  }
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
          Pixel p = {(unsigned char)d[dist[n]], (unsigned char)d[dist[n]], (unsigned char)d[dist[n]] };
          get_pixel(n.first, n.second) = p;
        }
        to_visit.push(n);
      }
  }
}
int main(void)
{
  read_image("paw.ppm");
  calculate_transport();
  calculate_blur();
  write_image("paw2.ppm");
}
