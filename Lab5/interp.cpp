// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

static unordered_map<string, void *> fontcode{
    {"Fixed-8x13", GLUT_BITMAP_8_BY_13},
    {"Fixed-9x15", GLUT_BITMAP_9_BY_15},
    {"Helvetica-10", GLUT_BITMAP_HELVETICA_10},
    {"Helvetica-12", GLUT_BITMAP_HELVETICA_12},
    {"Helvetica-18", GLUT_BITMAP_HELVETICA_18},
    {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
    {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};
unordered_map<string, interpreter::interpreterfn>
    interpreter::interp_map{
        {"define", &interpreter::do_define},
        {"draw", &interpreter::do_draw},
        {"border", &interpreter::do_border},
        {"moveby", &interpreter::do_moveby},
    };

unordered_map<string, interpreter::factoryfn>
    interpreter::factory_map{
        {"text", &interpreter::make_text},
        {"ellipse", &interpreter::make_ellipse},
        {"circle", &interpreter::make_circle},
        {"polygon", &interpreter::make_polygon},
        {"rectangle", &interpreter::make_rectangle},
        {"square", &interpreter::make_square},
        {"diamond", &interpreter::make_diamond},
        {"triangle", &interpreter::make_triangle},
        {"equilateral", &interpreter::make_equilateral},
    };

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter()
{
  for (const auto &itor : objmap)
  {
    cout << "objmap[" << itor.first << "] = "
         << *itor.second << endl;
  }
}

void interpreter::interpret(const parameters &params)
{
  DEBUGF('i', params);
  param begin = params.cbegin();
  string command = *begin;
  auto itor = interp_map.find(command);
  if (itor == interp_map.end())
    throw runtime_error("syntax error");
  interpreterfn func = itor->second;
  func(++begin, params.cend());
}

void interpreter::do_define(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  string name = *begin;
  objmap.emplace(name, make_shape(++begin, end));
}

void interpreter::do_draw(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  if (end - begin != 4)
    throw runtime_error("syntax error");
  string name = begin[1];
  shape_map::const_iterator itor = objmap.find(name);
  if (itor == objmap.end())
  {
    throw runtime_error(name + ": no such shape");
  }
  rgbcolor color{begin[0]};
  vertex where{from_string<GLfloat>(begin[2]),
               from_string<GLfloat>(begin[3])};
  window::push_back(object(itor->second, where, color));
}

void interpreter::do_border(param begin, param end)
{
  if (end - begin != 2)
    throw runtime_error("syntax error");
  rgbcolor color{begin[0]};
  size_t thicknes{stoul(begin[1])};
  window::setBorder(color, thicknes);
}

void interpreter::do_moveby(param begin, param end)
{
  if (end - begin != 1)
    throw runtime_error("syntax error");
  size_t moveby{stoul(begin[0])};
  window::setMoveBy(moveby);
}

shape_ptr interpreter::make_shape(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  string type = *begin++;
  auto itor = factory_map.find(type);
  if (itor == factory_map.end())
  {
    throw runtime_error(type + ": no such shape");
  }
  factoryfn func = itor->second;
  return func(begin, end);
}

shape_ptr interpreter::make_text(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  cout << "font: " << begin->c_str() << endl;
  auto font = fontcode.at(begin->c_str());
  ++begin;
  string textdata;
  for (; begin != end; ++begin)
  {
    textdata += begin->c_str();
    textdata += string(" ");
  }
  return make_shared<text>(font, textdata);
}

shape_ptr interpreter::make_ellipse(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  auto width = stof(begin->c_str());
  ++begin;
  auto height = stof(begin->c_str());

  return make_shared<ellipse>(GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_circle(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  auto diameter = stof(begin->c_str());
  return make_shared<circle>(GLfloat(diameter));
}

shape_ptr interpreter::make_polygon(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  vertex_list verticies;
  vertex v;
  float totalx = 0.0;
  float totaly = 0.0;
  for (; begin != end; ++begin)
  {
    v.xpos = stof(begin->c_str());
    totalx += v.xpos;
    ++begin;
    v.ypos = stof(begin->c_str());
    totaly += v.ypos;
    verticies.push_back(v);
  }

  float avgx = totalx / verticies.size();
  float avgy = totaly / verticies.size();

  for (auto vertex = verticies.begin(); vertex != verticies.end(); ++vertex)
  {
    vertex->xpos = vertex->xpos - avgx;
    vertex->ypos = vertex->ypos - avgy;
  }
  return make_shared<polygon>(verticies);
}

shape_ptr interpreter::make_rectangle(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  auto width = stof(begin->c_str());
  ++begin;
  auto height = stof(begin->c_str());
  return make_shared<rectangle>(GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_square(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  auto width = stof(begin->c_str());
  return make_shared<square>(GLfloat(width));
}

shape_ptr interpreter::make_diamond(param begin, param end)
{
  DEBUGF('f', range(begin, end));
  auto width = stof(begin->c_str());
  ++begin;
  auto height = stof(begin->c_str());
  return make_shared<diamond>(GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_triangle(param begin, param end)
{
  vertex_list verticies;
  vertex v;
  float totalx = 0.0;
  float totaly = 0.0;
  for (; begin != end; ++begin)
  {
    v.xpos = stof(begin->c_str());
    totalx += v.xpos;
    ++begin;
    v.ypos = stof(begin->c_str());
    totaly += v.ypos;
    verticies.push_back(v);
  }

  float avgx = totalx / verticies.size();
  float avgy = totaly / verticies.size();

  for (auto vertex = verticies.begin(); vertex != verticies.end(); ++vertex)
  {
    vertex->xpos = vertex->xpos - avgx;
    vertex->ypos = vertex->ypos - avgy;
  }
  return make_shared<triangle>(verticies);
}

shape_ptr interpreter::make_equilateral(param begin, param)
{
  auto width = stof(begin->c_str());
  return make_shared<equilateral>(width);
}
