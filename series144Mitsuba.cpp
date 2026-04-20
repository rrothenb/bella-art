// series144Mitsuba.cpp
// C++ port of series144.go (fm-animations).
// Dependencies on geom.Mtx (matrix inverse / MultPoint) are stubs because invisible() returns
// false unconditionally — the code after that return is dead in the original Go file too.

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <sys/stat.h>

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
// Vec3
//=================================================================================================

struct Vec3
{
    double x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s)      const { return {x*s,   y*s,   z*s  }; }

    double len()  const { return sqrt(x*x + y*y + z*z); }
    Vec3 unit()   const { double l = len(); return l > 1e-10 ? Vec3{x/l, y/l, z/l} : Vec3{0,0,0}; }
    Vec3 scaled(double s) const { return {x*s, y*s, z*s}; }
    Vec3 minus(const Vec3& o) const { return *this - o; }
    Vec3 plus(const Vec3& o)  const { return *this + o; }

    Vec3 cross(const Vec3& o) const
    {
        return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x};
    }
};

//=================================================================================================
// Mat4 — stub; only needed by dead code inside invisible() which returns false immediately.
//=================================================================================================

struct Mat4
{
    double m[4][4] = {};

    // STUB: inverse is never called (invisible() returns false unconditionally)
    Mat4 inverse() const { return *this; }

    // STUB: multPoint is never called for the same reason
    Vec3 multPoint(Vec3 p) const { return p; }
};

static Mat4 mat4Shift(Vec3 o)
{
    Mat4 r;
    r.m[0][0] = 1; r.m[1][1] = 1; r.m[2][2] = 1; r.m[3][3] = 1;
    r.m[3][0] = o.x; r.m[3][1] = o.y; r.m[3][2] = o.z;
    return r;
}

static Mat4 mat4Mult(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k)
                r.m[i][j] += a.m[i][k] * b.m[k][j];
    return r;
}

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
// SLR2 camera
//=================================================================================================

static const Vec3 zAxis = {0, 1, 0};

static Mat4 lookMatrix(Vec3 o, Vec3 to)
{
    Vec3 f = (o - to).unit();           // forward
    Vec3 r = zAxis.cross(f).unit();     // right (zAxis.Cross(f) in Go)
    Vec3 u = f.cross(r).unit();         // up
    Mat4 orient;
    orient.m[0][0] = r.x; orient.m[0][1] = u.x; orient.m[0][2] = f.x; orient.m[0][3] = 0;
    orient.m[1][0] = r.y; orient.m[1][1] = u.y; orient.m[1][2] = f.y; orient.m[1][3] = 0;
    orient.m[2][0] = r.z; orient.m[2][1] = u.z; orient.m[2][2] = f.z; orient.m[2][3] = 0;
    orient.m[3][0] = 0;   orient.m[3][1] = 0;   orient.m[3][2] = 0;   orient.m[3][3] = 1;
    return mat4Mult(mat4Shift(o), orient);
}

struct SLR2
{
    double aspectRatio = 1.0;
    double lens        = 0.050;
    double fStop       = 4.0;
    double focus       = 1.0;
    double FOV         = 0.0;

    Mat4 trans;
    Vec3 position = {0, 0, 0};
    Vec3 target   = {0, 0, -5};

    SLR2()
    {
        printf("constructor\n");
        transform();
    }

    SLR2* lookAt(Vec3 t)  { target   = t; printf("LookAt\n");  transform(); return this; }
    SLR2* moveTo(Vec3 p)  { position = p; printf("MoveTo\n");  transform(); return this; }

    void transform() { trans = lookMatrix(position, target); }

    bool invisible(Vec3 /*point*/) const
    {
        return false;
        // Dead code below — faithfully ported from Go (invisible() always returned false)
        Mat4 cameraSpaceTransform = trans.inverse();
        Vec3 projectedPoint = cameraSpaceTransform.multPoint({0,0,0}); // stub arg
        double factor = tan(FOV * 1.5 / 360 * pi);
        if (projectedPoint.x < projectedPoint.z * factor * aspectRatio ||
            projectedPoint.x > -projectedPoint.z * factor * aspectRatio)
            return true;
        if (projectedPoint.y < projectedPoint.z * factor ||
            projectedPoint.y > -projectedPoint.z * factor)
            return true;
        if (projectedPoint.z < -1.0)
            return true;
        return false;
        if (projectedPoint.y < projectedPoint.z * factor ||
            projectedPoint.y > -projectedPoint.z * factor)
            return true;
        return false;
    }
};

static SLR2* newSLR2() { return new SLR2(); }

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
    return cameraPath(t + .1) - Vec3{0, .1, 0};
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

static Vec3 cube(double u, double v, double /*t*/)
{
    return {
        sin(v/2.0 + .6*sin(v)) * cos(u - .6*sin(2*u)),
        sin(v/2.0 + .6*sin(v)) * sin(u + .6*sin(2*u)),
        cos(v/2.0 - .6*sin(v))
    };
}

static double metalBlendValue(double t, Vec3 loc)
{
    return spow(pow(shapeTexture(2, 1, t, loc)/2 + .5, 10)*2 - 1, .1) / 2 + .5;
}

static double blendValue(double u, double v, double t)
{
    return shaper(texture(u, v, t), pow(4, sin(71*t)), pow(2, cos(73*t)) - 1) / 2 + .5;
}

static Vec3 shape(double u, double v, double t)
{
    return cube(u, v, t).scaled(1 + .05*shaper(texture(u, v, t), pow(2, sin(31*t)), pow(3, sin(37*t) - 1)));
}

static Vec3 uv2xyz(double u, double v, double t)
{
    return shape(u, v, t);
}

//=================================================================================================
// UV helpers
//=================================================================================================

static double index2radians(double index, int n)
{
    return index / double(n) * pi * 2;
}

static Vec3 uvIndexToNormal(int uIndex, int vIndex, int nU, int nV, double t)
{
    Vec3 left  = uv2xyz(index2radians(double(uIndex) - .1, nU), index2radians(double(vIndex),       nV), t);
    Vec3 right = uv2xyz(index2radians(double(uIndex) + .1, nU), index2radians(double(vIndex),       nV), t);
    Vec3 up    = uv2xyz(index2radians(double(uIndex),       nU), index2radians(double(vIndex) + .1, nV), t);
    Vec3 down  = uv2xyz(index2radians(double(uIndex),       nU), index2radians(double(vIndex) - .1, nV), t);
    return (left - right).cross(up - down).unit();
}

//=================================================================================================
// RGBE (.rgbe) file writer — matches github.com/Opioid/rgbe Encode output.
// data[] is packed R,G,B float32 triples, width*height entries.
//=================================================================================================

static void rgbeEncode(float r, float g, float b, uint8_t out[4])
{
    float maxComp = r > g ? (r > b ? r : b) : (g > b ? g : b);
    if (maxComp < 1e-32f) { out[0] = out[1] = out[2] = out[3] = 0; return; }
    int   e;
    float scale = frexpf(maxComp, &e) * 256.0f / maxComp;
    out[0] = (uint8_t)(r * scale);
    out[1] = (uint8_t)(g * scale);
    out[2] = (uint8_t)(b * scale);
    out[3] = (uint8_t)(e + 128);
}

static void rgbeWrite(const char* path, int width, int height, const std::vector<float>& data)
{
    FILE* f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path); return; }
    fprintf(f, "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y %d +X %d\n", height, width);
    for (int i = 0; i < width * height; ++i)
    {
        uint8_t pixel[4];
        rgbeEncode(data[i*3+0], data[i*3+1], data[i*3+2], pixel);
        fwrite(pixel, 1, 4, f);
    }
    fclose(f);
}

//=================================================================================================
// PLY binary helpers — little-endian (native on x86 / Apple Silicon).
//=================================================================================================

static void plyWriteF32(FILE* f, float v)  { fwrite(&v, 4, 1, f); }
static void plyWriteI32(FILE* f, int32_t v) { fwrite(&v, 4, 1, f); }
static void plyWriteU8 (FILE* f, uint8_t v) { fwrite(&v, 1, 1, f); }

//=================================================================================================
// Mitsuba sensor.xml writer — direct fprintf replacing Go's text/template.
//=================================================================================================

static void writeSensorXml(
    FILE* f,
    Vec3 camera, Vec3 lookAt,
    double distance, double fov, double aperture,
    int width, int height, int samples, int rowHeight,
    double intIOR,
    Vec3 eta, Vec3 k,
    double g, double scale,
    int weight1, int weight2,
    double alpha1, double alpha2)
{
    fprintf(f,
"\n<scene version=\"2.0.0\">\n"
"    <sensor type=\"thinlens\" id=\"Camera-camera\">\n"
"        <string name=\"fov_axis\" value=\"larger\"/>\n"
"        <float name=\"focus_distance\" value=\"%g\"/>\n"
"        <float name=\"aperture_radius\" value=\"%g\"/>\n"
"        <float name=\"fov\" value=\"%g\"/>\n"
"        <transform name=\"to_world\">\n"
"            <lookat origin=\"%g, %g, %g\" target=\"%g, %g, %g\" up=\"0, 1, 0\"/>\n"
"        </transform>\n"
"\n"
"        <sampler type=\"multijitter\">\n"
"            <integer name=\"sample_count\" value=\"%d\"/>\n"
"        </sampler>\n"
"\n"
"        <film type=\"hdrfilm\" id=\"film\">\n"
"            <integer name=\"width\" value=\"%d\"/>\n"
"            <integer name=\"height\" value=\"%d\"/>\n"
"            <integer name=\"crop_offset_y\" value=\"$offset\"/>\n"
"            <integer name=\"crop_height\" value=\"%d\"/>\n"
"            <rfilter type=\"lanczos\"/>\n"
"        </film>\n"
"    </sensor>\n"
"    <emitter type=\"envmap\" id=\"Area_002-light\">\n"
"        <string name=\"filename\" value=\"mitsuba.rgbe\"/>\n"
"        <float name=\"scale\" value=\"1\"/>\n"
"        <transform name=\"to_world\">\n"
"            <rotate value=\"1, 0, 0\" angle=\"-90\"/>\n"
"        </transform>\n"
"    </emitter>\n"
"    <integrator type=\"volpathmis\">\n"
"        <integer name=\"max_depth\" value=\"16\"/>\n"
"    </integrator>\n"
"    <medium id=\"medium1\" type=\"homogeneous\">\n"
"        <float name=\"scale\" value=\"%g\"/>\n"
"        <rgb name=\"sigma_t\" value=\"%g, %g, %g\"/>\n"
"        <rgb name=\"albedo\" value=\"%g, %g, %g\"/>\n"
"        <phase type=\"hg\">\n"
"            <float name=\"g\" value=\"%g\"/>\n"
"        </phase>\n"
"    </medium>\n"
"    <bsdf type=\"blendbsdf\" id=\"object_bsdf\">\n"
"        <float name=\"weight\" value=\"%d\"/>\n"
"        <bsdf type=\"blendbsdf\">\n"
"            <float name=\"weight\" value=\"%d\"/>\n"
"            <bsdf type=\"blendbsdf\">\n"
"                <texture type=\"bitmap\" name=\"weight\">\n"
"                    <string name=\"filename\" value=\"mitsuba.blend.rgbe\"/>\n"
"                </texture>\n"
"                <bsdf type=\"twosided\">\n"
"                    <bsdf type=\"roughconductor\">\n"
"                        <float name=\"alpha\" value=\"%g\"/>\n"
"                        <rgb name=\"k\" value=\"%g, %g, %g\"/>\n"
"                        <rgb name=\"eta\" value=\"%g, %g, %g\"/>\n"
"                    </bsdf>\n"
"                </bsdf>\n"
"                <bsdf type=\"roughdielectric\">\n"
"                    <float name=\"alpha\" value=\"%g\"/>\n"
"                    <float name=\"int_ior\" value=\"2\"/>\n"
"                    <float name=\"ext_ior\" value=\"%g\"/>\n"
"                </bsdf>\n"
"            </bsdf>\n"
"            <bsdf type=\"blendbsdf\">\n"
"                <texture type=\"bitmap\" name=\"weight\">\n"
"                    <string name=\"filename\" value=\"mitsuba.blend.rgbe\"/>\n"
"                </texture>\n"
"                <bsdf type=\"roughdielectric\">\n"
"                    <float name=\"alpha\" value=\"%g\"/>\n"
"                    <float name=\"int_ior\" value=\"2\"/>\n"
"                    <float name=\"ext_ior\" value=\"%g\"/>\n"
"                </bsdf>\n"
"                <bsdf type=\"twosided\">\n"
"                    <bsdf type=\"roughconductor\">\n"
"                        <float name=\"alpha\" value=\"%g\"/>\n"
"                        <rgb name=\"k\" value=\"%g, %g, %g\"/>\n"
"                        <rgb name=\"eta\" value=\"%g, %g, %g\"/>\n"
"                    </bsdf>\n"
"                </bsdf>\n"
"            </bsdf>\n"
"        </bsdf>\n"
"        <bsdf type=\"blendbsdf\">\n"
"            <float name=\"weight\" value=\"%d\"/>\n"
"            <bsdf type=\"blendbsdf\">\n"
"                <texture type=\"bitmap\" name=\"weight\">\n"
"                    <string name=\"filename\" value=\"mitsuba.blend.rgbe\"/>\n"
"                </texture>\n"
"                <bsdf type=\"twosided\">\n"
"                    <bsdf type=\"diffuse\">\n"
"                        <rgb name=\"reflectance\" value=\"%g, %g, %g\"/>\n"
"                    </bsdf>\n"
"                </bsdf>\n"
"                <bsdf type=\"twosided\">\n"
"                    <bsdf type=\"roughconductor\">\n"
"                        <float name=\"alpha\" value=\"%g\"/>\n"
"                        <rgb name=\"k\" value=\"%g, %g, %g\"/>\n"
"                        <rgb name=\"eta\" value=\"%g, %g, %g\"/>\n"
"                    </bsdf>\n"
"                </bsdf>\n"
"            </bsdf>\n"
"            <bsdf type=\"blendbsdf\">\n"
"                <texture type=\"bitmap\" name=\"weight\">\n"
"                    <string name=\"filename\" value=\"mitsuba.blend.rgbe\"/>\n"
"                </texture>\n"
"                <bsdf type=\"roughdielectric\">\n"
"                    <float name=\"alpha\" value=\"%g\"/>\n"
"                    <float name=\"int_ior\" value=\"2\"/>\n"
"                    <float name=\"ext_ior\" value=\"%g\"/>\n"
"                </bsdf>\n"
"                <bsdf type=\"twosided\">\n"
"                    <bsdf type=\"diffuse\">\n"
"                        <rgb name=\"reflectance\" value=\"%g, %g, %g\"/>\n"
"                    </bsdf>\n"
"                </bsdf>\n"
"            </bsdf>\n"
"        </bsdf>\n"
"    </bsdf>\n"
"    <shape type=\"ply\">\n"
"        <string name=\"filename\" value=\"mitsuba.ply\"/>\n"
"        <transform name=\"to_world\">\n"
"            <scale value=\"1\"/>\n"
"            <translate x=\"0\" y=\"0\" z=\"0\"/>\n"
"        </transform>\n"
"        <ref id=\"object_bsdf\"/>\n"
"        <ref id=\"medium1\" name=\"interior\"/>\n"
"    </shape>\n"
"</scene>\n",
        // sensor
        distance, aperture, fov,
        camera.x, camera.y, camera.z,
        lookAt.x, lookAt.y, lookAt.z,
        samples,
        width, height, rowHeight,
        // medium
        scale,
        eta.x, eta.y, eta.z,
        k.x, k.y, k.z,
        g,
        // bsdf block 1
        weight1,
        weight2,
        // blendbsdf > blendbsdf > roughconductor
        alpha2, eta.x, eta.y, eta.z, k.x, k.y, k.z,
        // roughdielectric
        alpha1, intIOR,
        // blendbsdf > blendbsdf > roughdielectric (second inner)
        alpha1, intIOR,
        // roughconductor (second inner)
        alpha2, eta.x, eta.y, eta.z, k.x, k.y, k.z,
        // bsdf block 2
        weight2,
        // diffuse reflectance
        k.x, k.y, k.z,
        // roughconductor
        alpha2, eta.x, eta.y, eta.z, k.x, k.y, k.z,
        // roughdielectric
        alpha1, intIOR,
        // diffuse reflectance (second)
        eta.x, eta.y, eta.z
    );
}

//=================================================================================================
// renderSurfaces
//=================================================================================================

static void renderSurfaces(int frameNumber, int pixels, int maxSubdivisions,
                           double dt, int desiredTriangles, double aspectRatio,
                           int height, int samples, int numRows)
{
    int    width     = int(aspectRatio * double(height));
    double t         = double(frameNumber) * dt;
    int    envSize   = int(pow(double(desiredTriangles), .5));
    Vec3   cameraLoc = cameraPath(t);
    Vec3   focusPoint = focusPath(t);
    double fov       = 136.0;

    SLR2* c = newSLR2();
    c->moveTo(cameraLoc);
    c->lookAt(focusPoint);
    c->FOV         = fov;
    c->aspectRatio = aspectRatio;

    double distance = .25;
    printf("\ncameraLoc: (%g,%g,%g)\nfocusPoint: (%g,%g,%g)\ndistance: %g\nt: %g\n",
           cameraLoc.x, cameraLoc.y, cameraLoc.z,
           focusPoint.x, focusPoint.y, focusPoint.z,
           distance, t);

    int nU = int(double(pixels) / distance * 3);
    if (nU > maxSubdivisions) nU = maxSubdivisions;
    int nV = nU;

    int    numTriangles  = 0;
    double totalWidth    = 0.0;
    double totalHeight   = 0.0;
    double minDistance   = 100.0;
    double maxDistance   = 0.0;
    double maxX = 0.0, maxY = 0.0, maxZ = 0.0;
    int    minU = 500, minV = 500, maxU = 0, maxV = 0;
    double minZ = 1.0;
    Vec3   closestPoint  = {0, 0, 0};

    for (int uIndex = 0; uIndex <= 500; ++uIndex)
    {
        for (int vIndex = 0; vIndex <= 500; ++vIndex)
        {
            Vec3 vertex = uv2xyz(index2radians(double(uIndex), 500),
                                 index2radians(double(vIndex), 500), t);
            if (!c->invisible(vertex))
            {
                numTriangles++;
                Vec3 vertexLeft  = uv2xyz(index2radians(double(uIndex-1), 500),
                                          index2radians(double(vIndex),   500), t);
                Vec3 vertexBelow = uv2xyz(index2radians(double(uIndex),   500),
                                          index2radians(double(vIndex-1), 500), t);
                totalWidth  += (vertex - vertexLeft).len();
                totalHeight += (vertex - vertexBelow).len();
                minDistance  = min(minDistance, vertex.len());
                maxDistance  = max(maxDistance, vertex.len());
                maxX = max(maxX, abs(vertex.x));
                maxY = max(maxY, abs(vertex.y));
                maxZ = max(maxZ, abs(vertex.z));
                minZ = min(minZ, vertex.z);
                if (minU > uIndex) minU = uIndex;
                if (minV > vIndex) minV = vIndex;
                if (maxU < uIndex) maxU = uIndex;
                if (maxV < vIndex) maxV = vIndex;
                if ((cameraLoc - closestPoint).len() > (cameraLoc - vertex).len())
                    closestPoint = vertex;
            }
        }
    }

    distance = (cameraLoc - closestPoint).len();
    printf("minDistance: %g, maxDistance: %g, distance: %g, len: %g, maxX: %g, maxY: %g, maxZ: %g, minZ: %g\n",
           minDistance, maxDistance, distance, cameraLoc.len(), maxX, maxY, maxZ, minZ);

    double ratio = totalWidth / totalHeight;
    printf("%g %g %g\n", totalWidth, totalHeight, ratio);
    printf("%d\n", numTriangles);

    double scale2 = sqrt(double(desiredTriangles) / double(numTriangles*2));
    nU = int(scale2 * sqrt(ratio) * 500);
    nV = int(scale2 / sqrt(ratio) * 500);
    int startUIndex = int(scale2 * sqrt(ratio) * double(minU));
    int endUIndex   = int(scale2 * sqrt(ratio) * double(maxU));
    int startVIndex = int(scale2 / sqrt(ratio) * double(minV));
    int endVIndex   = int(scale2 / sqrt(ratio) * double(maxV));

    printf("distance from center: %g, distance from focal point: %g, nU: %d, nV: %d\n",
           cameraLoc.len(), distance, nU, nV);

    // Vertex index table: [uIndex][vIndex] = vertex index in PLY, or -1 if culled.
    std::vector<std::vector<int32_t>> vertexIndicies(nU + 1);
    int numVerticies = 0;

    mkdir("data", 0755);
    char plyDataPath[256];
    snprintf(plyDataPath, sizeof(plyDataPath), "data/%d.data.ply", frameNumber);
    FILE* plyData = fopen(plyDataPath, "wb");

    for (int uIndex = startUIndex; uIndex <= endUIndex; ++uIndex)
    {
        vertexIndicies[uIndex].assign(nV + 1, -1);
        for (int vIndex = startVIndex; vIndex <= endVIndex; ++vIndex)
        {
            Vec3 vertex = uv2xyz(index2radians(double(uIndex), nU),
                                 index2radians(double(vIndex), nV), t);
            if (c->invisible(vertex))
                continue;
            Vec3 normal = uvIndexToNormal(uIndex, vIndex, nU, nV, t);
            vertexIndicies[uIndex][vIndex] = int32_t(numVerticies++);
            plyWriteF32(plyData, float(vertex.x));
            plyWriteF32(plyData, float(vertex.y));
            plyWriteF32(plyData, float(vertex.z));
            plyWriteF32(plyData, float(normal.x));
            plyWriteF32(plyData, float(normal.y));
            plyWriteF32(plyData, float(normal.z));
            plyWriteF32(plyData, float(index2radians(double(uIndex - startUIndex),
                                                     endUIndex - startUIndex) / pi / 2));
            plyWriteF32(plyData, float(index2radians(double(vIndex - startVIndex),
                                                     endVIndex - startVIndex) / pi / 2));
        }
    }

    std::vector<float> envmapArray, blendArray, metalBlendArray;
    int numFaces = 0;

    for (int vIndex = startVIndex; vIndex < endVIndex; ++vIndex)
    {
        for (int uIndex = startUIndex; uIndex < endUIndex; ++uIndex)
        {
            double u   = double(uIndex) / double(nU) * 2 * pi;
            double v   = double(vIndex) / double(nV) * 2 * pi;
            Vec3   loc = shape(u, v, t);

            // Local bv / mbv to avoid shadowing the blendValue / metalBlendValue functions.
            float bv  = float(blendValue(u, v, t));
            blendArray.push_back(bv); blendArray.push_back(bv); blendArray.push_back(bv);
            float mbv = float(metalBlendValue(t, loc));
            metalBlendArray.push_back(mbv); metalBlendArray.push_back(mbv); metalBlendArray.push_back(mbv);

            int32_t topRight = vertexIndicies[uIndex  ][vIndex  ];
            int32_t topLeft  = vertexIndicies[uIndex+1][vIndex  ];
            int32_t botRight = vertexIndicies[uIndex  ][vIndex+1];
            int32_t botLeft  = vertexIndicies[uIndex+1][vIndex+1];

            if (topRight == -1 || topLeft == -1 || botRight == -1 || botLeft == -1)
                continue;

            plyWriteU8 (plyData, 3);
            plyWriteI32(plyData, topRight);
            plyWriteI32(plyData, botLeft);
            plyWriteI32(plyData, topLeft);
            numFaces++;

            plyWriteU8 (plyData, 3);
            plyWriteI32(plyData, topRight);
            plyWriteI32(plyData, botRight);
            plyWriteI32(plyData, botLeft);
            numFaces++;
        }
    }

    for (int vIndex = 0; vIndex < envSize; ++vIndex)
    {
        for (int uIndex = 0; uIndex < envSize; ++uIndex)
        {
            double eu  = double(uIndex) / double(envSize) * 2 * pi;
            double ev  = double(vIndex) / double(envSize) * pi;
            float  val = float(pow(sin(eu/2), pow(2, cos(t)+3)) * pow(sin(ev), pow(2, cos(t)+3)));
            envmapArray.push_back(val); envmapArray.push_back(val); envmapArray.push_back(val);
        }
    }

    printf("Mitsuba!\n");
    fclose(plyData);

    // PLY header (written separately; cat header.ply data.ply > mitsuba.ply to combine)
    char plyHeaderPath[256];
    snprintf(plyHeaderPath, sizeof(plyHeaderPath), "data/%d.header.ply", frameNumber);
    FILE* plyHeader = fopen(plyHeaderPath, "w");
    fprintf(plyHeader,
"\nply\n"
"format binary_little_endian 1.0\n"
"element vertex %d\n"
"property float32 x\n"
"property float32 y\n"
"property float32 z\n"
"property float32 nx\n"
"property float32 ny\n"
"property float32 nz\n"
"property float32 u\n"
"property float32 v\n"
"element face %d\n"
"property list uint8 int32 vertex_index\n"
"end_header\n",
        numVerticies, numFaces);
    fclose(plyHeader);

    char envPath[256], blendPath[256], metalBlendPath[256];
    snprintf(envPath,        sizeof(envPath),        "data/%d.rgbe",             frameNumber);
    snprintf(blendPath,      sizeof(blendPath),      "data/%d.blend.rgbe",       frameNumber);
    snprintf(metalBlendPath, sizeof(metalBlendPath), "data/%d.metal.blend.rgbe", frameNumber);

    rgbeWrite(envPath,        envSize,              envSize,              envmapArray);
    rgbeWrite(blendPath,      endUIndex-startUIndex, endVIndex-startVIndex, blendArray);
    rgbeWrite(metalBlendPath, endUIndex-startUIndex, endVIndex-startVIndex, metalBlendArray);

    // Color parameters for materials
    double s1 = cos(41*t)*.4 + .5;
    double b1 = .5 - sin(43*t)*.4;
    double s2 = s1 + .4;
    if (s2 > .9) s2 = s1 - .4;
    double b2 = b1 + .4;
    if (b2 > .9) b2 = b1 - .4;
    double h1  = 47 * t / 2 / pi;
    double h2  = h1 + .5 + sin(t)*.25;
    Vec3   eta = hsb2rgb(h1, s1, b1);
    Vec3   k   = hsb2rgb(h2, s2, b2);

    FILE* sensorFile = fopen("sensor.xml", "w");
    writeSensorXml(
        sensorFile,
        cameraLoc, focusPoint,
        distance, fov, .000000000001,
        width, height, samples, height / numRows,
        cos(59*t) + 2.5,
        eta, k,
        -cos(61*t) * .9,
        100,
        0, 0,
        pow(10, sin(67*t) - 2),
        pow(10, cos(79*t) - 2)
    );
    fclose(sensorFile);

    delete c;
}

//=================================================================================================
// main — argument parsing mirrors Go's flag package defaults
//=================================================================================================

static int parseIntArg(int argc, char** argv, const char* flag, int defaultVal)
{
    for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], flag) == 0)
            return atoi(argv[i+1]);
    return defaultVal;
}

static double parseDoubleArg(int argc, char** argv, const char* flag, double defaultVal)
{
    for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], flag) == 0)
            return atof(argv[i+1]);
    return defaultVal;
}

int main(int argc, char** argv)
{
    int    frame           = parseIntArg   (argc, argv, "--frame",           0);
    int    pixels          = parseIntArg   (argc, argv, "--pixels",          256);
    int    maxSubdivisions = parseIntArg   (argc, argv, "--maxsubdivisions", 1000);
    int    maxFrames       = parseIntArg   (argc, argv, "--maxframes",       1000);
    int    desiredTriangles= parseIntArg   (argc, argv, "--desiredtriangles",0);
    double aspectRatio     = parseDoubleArg(argc, argv, "--aspectratio",     1.0);
    int    height          = parseIntArg   (argc, argv, "--height",          720);
    int    samples         = parseIntArg   (argc, argv, "--samples",         25);
    int    numRows         = parseIntArg   (argc, argv, "--numrows",         1);

    printf("frame: %d, pixels: %d, maxSubdivisions: %d, maxFrames: %d\n",
           frame, pixels, maxSubdivisions, maxFrames);

    double dt = pi * 2 / double(maxFrames);
    renderSurfaces(frame, pixels, maxSubdivisions, dt, desiredTriangles,
                   aspectRatio, height, samples, numRows);
    return 0;
}
