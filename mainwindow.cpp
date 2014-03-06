#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "noiseutils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_img = new QImage(QSize(512, 512), QImage::Format_RGB32);
    m_centralWidget = new QLabel;
    ui->setupUi(this);
    setCentralWidget(m_centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionGenerateNoise_triggered()
{
    noise::module::RidgedMulti noiseModule;
    noise::module::Clamp clampModule;
    clampModule.SetBounds(-1.0, 1.0);

    //noiseModule.SetOctaveCount(8);
    noiseModule.SetSeed(124);
    noise::utils::NoiseMap noiseMap;
    noise::utils::NoiseMapBuilderSphere sphericalBuilder;
    sphericalBuilder.SetDestNoiseMap(noiseMap);
    sphericalBuilder.SetDestSize(512, 512);
    sphericalBuilder.SetBounds(-90, 90, -180, 180);
    float min = 0;
    float max = 0;

    uchar* imageBits = m_img->bits();

    for (int i = 0; i < 16; ++i)
    {
        noiseModule.SetSeed(74 + i);
        clampModule.SetSourceModule(0, noiseModule);
        sphericalBuilder.SetSourceModule(clampModule);
        sphericalBuilder.Build();
        QRgb* currPixel = reinterpret_cast<QRgb*>(imageBits);
        for (unsigned int y = 0; y < 512; ++y)
            for (unsigned int x = 0; x < 512; ++x)
            {
                float noiseVal = (noiseMap.GetValue(x, y) + 1)*0.5;
                *(currPixel++) = qRgb(255 * noiseVal, 255 * noiseVal, 255 * noiseVal);
                if (min > noiseVal)
                    min = noiseVal;
                if (max < noiseVal)
                    max = noiseVal;
            }
        m_img->save(tr("sphericalnoise%1.png").arg(i));
        m_centralWidget->setPixmap(QPixmap::fromImage(*m_img));
    }
}
