// series144Mitsuba.cpp
// C++ port of the math functions from series144.go (fm-animations).
// No rendering pipeline — just geometry and texture helpers for copy-paste use.

#include <cmath>
#include <algorithm>

using std::sin;
using std::cos;
using std::tan;
using std::pow;
using std::sqrt;
using std::abs;
using std::floor;
using std::min;
using std::max;

static const double pi = M_PI;

//=================================================================================================
// Minimal Vec3
//=================================================================================================

struct Vec3
{
    double x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s)      const { return {x*s,   y*s,   z*s  }; }

    double len()   const { return sqrt(x*x + y*y + z*z); }
    Vec3   unit()  const { double l = len(); return l > 1e-10 ? Vec3{x/l, y/l, z/l} : Vec3{0,0,0}; }
    Vec3   scaled(double s) const { return {x*s, y*s, z*s}; }
    Vec3   plus(const Vec3& o)  const { return *this + o; }
    Vec3   minus(const Vec3& o) const { return *this - o; }

    Vec3 cross(const Vec3& o) const
    {
        return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x};
    }
};

//=================================================================================================
// Math helpers
//=================================================================================================

static double sign(double x) { return x < 0 ? -1.0 : 1.0; }

static double spow(double x, double y)
{
    return sign(x) * pow(abs(x), y);
}

static double pushout(double x, double duty, double degree)
{
    return spow(pow(x, duty)*2 - 1, degree) / 2 + .5;
}

static double strength(int n, double x)
{
    return pow(8, sin(double(n) * (x + double(n) / 10)));
}

//=================================================================================================
// Color
//=================================================================================================

static Vec3 hsb2rgb(double hue, double sat, double bri)
{
    double u = bri;
    if (sat == 0.0) return {u, u, u};
    double h  = (hue - floor(hue)) * 6.0;
    double f  = h - floor(h);
    double p  = bri * (1.0 - sat);
    double q  = bri * (1.0 - sat * f);
    double t  = bri * (1.0 - sat * (1.0 - f));
    switch (int(h))
    {
    case 0: return {u, t, p};
    case 1: return {q, u, p};
    case 2: return {p, u, t};
    case 3: return {p, q, u};
    case 4: return {t, p, u};
    case 5: return {u, p, q};
    default: return {u, u, u};
    }
}

//=================================================================================================
// Surface geometry
//=================================================================================================

static Vec3 circle(double x)
{
    return {sin(x), cos(x), 0};
}

static double lipTexture(double u, double t)
{
    return sin(u + 2*strength(5,t)*sin(u + 2*strength(7,t)*sin(u))
                 + 2*strength(11,t)*sin(2*u)
                 + 2*strength(13,t)*sin(3*u));
}

static Vec3 bowl(double thickness, double insideTexture, double outsideTexture,
                 double u, double v, double t)
{
    double lip    = lipTexture(u, t);
    double width  = 1.0 + .1*strength(3,t)*pow(sin(v/2), 10)
                        * pow(spow(lip, pow(3, sin(2*t)))/2 + .5, pow(3, sin(3*t)));
    double height = sin(t)*.15 + .35 + .1*strength(2,t)*pow(sin(v/2), 10)
                        * pow(spow(lip, pow(3, sin(2*t)))/2 + .5, pow(3, sin(3*t)));
    double space  = (cos(v/2 - .7*sin(v))/2 + .5) * (thickness + outsideTexture)
                  + (.5 - cos(v/2 - .7*sin(v))/2) * insideTexture;
    return {
        width * sin(u) * sin(v/2) * (1 + 1/height*space),
        width * cos(u) * sin(v/2) * (1 + 1/height*space),
        -height * cos(v - (sin(7*t)*.4 + .5)*sin(2*v)) * (1 + 1/height*space)
    };
}

static Vec3 torusKnot(double t, double R, double r, int p, int q, Vec3(*path)(double))
{
    Vec3   pp = path(double(q) * t);
    double rp = R + r * cos(double(p) * t);
    return {rp * pp.x, rp * pp.y, r * sin(double(p) * t) + pp.z};
}

static Vec3 lissajousKnot(double t, int xN, int yN, int zN)
{
    return {sin(double(xN)*t), sin(double(yN)*t), cos(double(zN)*t)};
}

static Vec3 unitLissajousKnot(double t, int xN, int yN, int zN)
{
    return lissajousKnot(t, xN, yN, zN).unit();
}

static Vec3 outerKnot(double t)
{
    return torusKnot(t, 1, .5, 3, 2, circle);
}

static Vec3 innerKnot(double t)
{
    return torusKnot(t, 1, .333, 2, 3, outerKnot);
}

static Vec3 lastKnot(double t)
{
    double scale = innerKnot(t).len();
    return torusKnot(t, 1, .05*scale, 100, 3, innerKnot);
}

static Vec3 cameraPath(double t)
{
    return {sin(t)*.5, 1.5 + cos(t)*.25, cos(t)*.5 - .25};
}

static Vec3 focusPath(double t)
{
    return (cameraPath(t + .1) - Vec3{0, .1, 0});
}

static Vec3 pathWrapper(double u, double v, double r, Vec3(*path)(double))
{
    double delta   = .01;
    Vec3   center  = path(v);
    r = r * center.len();
    Vec3 tangent = (path(v + delta) - path(v - delta)).unit();
    Vec3 sinVec  = tangent.cross({0, 0, 1}).unit();
    Vec3 cosVec  = tangent.cross(sinVec);
    return cosVec.scaled(r*cos(u)) + sinVec.scaled(r*sin(u)) + center;
}

static Vec3 knot(double t)
{
    return unitLissajousKnot(t, 19, 20, 21);
}

//=================================================================================================
// Texture functions
//=================================================================================================

static double shapeTexture(double f, double a, double t, Vec3 loc)
{
    loc = loc.scaled(f * 2 * pi);
    return sin(loc.z
        + a*strength(7,t) *sin(a*strength(23,t)*loc.z)
        + a*strength(11,t)*sin(a*strength(19,t)*loc.z + a*strength(29,t)*sin(a*strength(31,t)*loc.y))
        + a*strength(13,t)*sin(a*strength(17,t)*loc.z)
        + a*strength(37,t)*sin(a*strength(41,t)*loc.x - a*strength(43,t)*loc.y)
        + a*strength(47,t)*sin(loc.z + a*strength(53,t)*sin(loc.x*loc.z + .1*a*strength(59,t)*sin(loc.z*loc.y))));
}

static double texture(double u, double v, double t)
{
    return sin(
        strength(2,t)*sin(11*u + strength(19,t)*sin(u))*sin(17*v + strength(23,t)*16*v)
      + strength(3,t)*sin(12*u + strength(17,t)*sin(v))
      + strength(5,t)*sin(13*v + strength(13,t)*sin(u))
      + strength(7,t)*sin(14*u + 15*v + strength(11,t)*sin(u + v + strength(29,t)*sin(u - v))));
}

static double shaper(double x, double a, double b)
{
    return spow(pow(x/2 + .5, a)*2 - 1, b);
}

static Vec3 cube(double u, double v, double t)
{
    return {
        sin(v/2.0 + .6*sin(v)) * cos(u - .6*sin(2*u)),
        sin(v/2.0 + .6*sin(v)) * sin(u + .6*sin(2*u)),
        cos(v/2.0 - .6*sin(v))
    };
}

static Vec3 shape(double u, double v, double t)
{
    return cube(u, v, t).scaled(1 + .05*shaper(texture(u, v, t), pow(2, sin(31*t)), pow(3, sin(37*t) - 1)));
}

static Vec3 uv2xyz(double u, double v, double t)
{
    return shape(u, v, t);
}

static double blendValue(double u, double v, double t)
{
    return shaper(texture(u, v, t), pow(4, sin(71*t)), pow(2, cos(73*t)) - 1) / 2 + .5;
}

static double metalBlendValue(double t, Vec3 loc)
{
    return spow(pow(shapeTexture(2, 1, t, loc)/2 + .5, 10)*2 - 1, .1) / 2 + .5;
}

//=================================================================================================
// UV index helpers
//=================================================================================================

static double index2radians(double index, int n)
{
    return index / double(n) * pi * 2;
}
