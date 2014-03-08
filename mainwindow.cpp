#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "noiseutils.h"
#include <qmath.h>
#include <QVector3D>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_heightmap = new QImage(QSize(512, 512), QImage::Format_RGB32);
    m_normalmap = new QImage(QSize(512, 512), QImage::Format_RGBA8888);
    m_centralWidget = new QLabel;
    ui->setupUi(this);
    setCentralWidget(m_centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

float bitmapValue(int x, int y, noise::utils::NoiseMap* map, int resolution)
{
    return map->GetValue((x%resolution), (y%resolution));
}

float getInterpolatedBitmapValue(noise::utils::NoiseMap* map, float  u, float v, int resolution)
{
    u = u*(resolution-1);
    v = v*(resolution-1);
    int su = u; //get floor(u)
    int sv = v; //get floor(v)

    float weightu = 1 - (u - su); //Anteil des vorderen
    float weightv = 1 - (v - sv);

    float sum = 0; //sums of values of 4 nearest texels

    for(int x = 0; x <= 1; x++)
        for(int y = 0; y <= 1; y++)
            sum += bitmapValue(su + x, sv + y, map, resolution) * abs(x-weightu) * abs(y-weightv);

    return sum;
}

void MainWindow::on_actionGenerateNoise_triggered()
{
    noise::module::RidgedMulti noiseModule;
    noise::module::Clamp clampModule;
    clampModule.SetBounds(0, 1.0);

    //noiseModule.SetOctaveCount(8);
    noiseModule.SetSeed(124);
    noise::utils::NoiseMap noiseMap;
    noise::utils::NoiseMapBuilderSphere sphericalBuilder;
    sphericalBuilder.SetDestNoiseMap(noiseMap);
    sphericalBuilder.SetDestSize(512, 512);
    sphericalBuilder.SetBounds(-90, 90, -180, 180);
    float min = 0;
    float max = 0;

    uchar* normalBits = m_normalmap->bits();

    for (int i = 0; i < 1; ++i)
    {
        noiseModule.SetSeed(74 + i);
        clampModule.SetSourceModule(0, noiseModule);
        sphericalBuilder.SetSourceModule(clampModule);
        sphericalBuilder.Build();

        for (unsigned int y = 0; y < 512; ++y)
            for (unsigned int x = 0; x < 512; ++x)
            {
                float u = x/512.0f; // lon <-> u
                float v = y/512.0f; // lat <-> v

                //get angle for c = (u,v)
                float lon = u * 2 * M_PI;
                float lat = v * 1 * M_PI;

                //get coordinates
                QVector3D a(
                    cos(lon) * (sin(lat)),
                    cos(lat),
                    sin(lon) * (sin(lat)));
                float delta = M_PI/512.0f;
                float deltalon = lon + delta*2;
                float deltalat = lat + delta;
                QVector3D b(
                    cos(deltalon) * (sin(lat)),
                    cos(lat),
                    sin(deltalon) * (sin(lat)));
                QVector3D c(
                    cos(lon) * (sin(deltalat)),
                    cos(deltalat),
                    sin(lon) * (sin(deltalat)));
                //get corresponding u,v coordinates
                float bu, cv;
                bu = deltalon / (2*M_PI);
                cv = deltalat / M_PI;

                //apply hightmap
                a *= 1+getInterpolatedBitmapValue(&noiseMap, u, v, 512);
                b *= 1+getInterpolatedBitmapValue(&noiseMap, bu, v, 512);
                c *= 1+getInterpolatedBitmapValue(&noiseMap, u, cv, 512);

                //calculate normal vector for spanned plane
                QVector3D normal = QVector3D::crossProduct(b-a,c-a);
                normal.normalize();

                //save normalized normal vector to texture. rgb = xyz
                int texeloffset = (y*512 + x)*4;
                normalBits[texeloffset + 0] = (normal.x()+1)*255/2.0f;
                normalBits[texeloffset + 1] = (normal.y()+1)*255/2.0f;
                normalBits[texeloffset + 2] = (normal.z()+1)*255/2.0f;
                normalBits[texeloffset + 3] = bitmapValue(x,y,&noiseMap,512)*255;
            }
        m_normalmap->save(tr("sphericalnoise%1.png").arg(i));
        m_centralWidget->setPixmap(QPixmap::fromImage(*m_normalmap));
    }
}
