#include "info.h"
#include "ui_info.h"

Info::Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
    hostname   = new QLabel("Hostname: ");
    user       = new QLabel("User: ");
    uptime     = new QLabel("Uptime: ");
    proc       = new QLabel("CPU: ");
    freq       = new QLabel("Frequency:");
    cpuload    = new QLabel("CPU load: ");
    mem        = new QLabel("RAM: ");
    memload    = new QLabel("USAGE RAM: ");
    cpubar     = new QProgressBar;
    membar     = new QProgressBar;
    layout     = new QVBoxLayout;
    hlayout    = new QHBoxLayout;

    cpubar->setMaximumHeight(21); membar->setMaximumHeight(21);
    hlayout->addWidget(cpuload); hlayout->addWidget(cpubar);
    layout->addWidget(hostname); layout->addWidget(user);
    layout->addWidget(uptime); layout->addWidget(proc);
    layout->addWidget(freq); layout->addLayout(hlayout);
    layout->addWidget(mem); layout->addWidget(memload);
    layout->addWidget(membar); setLayout(layout);

    update();

    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(2000);
}

Info::~Info()
{
    delete ui;
}



void Info::update()
{
    ifstream stream("/proc/sys/kernel/hostname"); string str;
    getline(stream,str);
    hostname->setText("Hostname: " + QString::fromStdString(str));
    uid_t uid = geteuid();
    passwd *pw = getpwuid(uid);
    user->setText("User: " + QString::fromAscii(pw->pw_name));
    struct sysinfo o;
    sysinfo(&o);
    long up = o.uptime;
    int hour = up/60/60;
    int min = (up - hour*60*60) / 60;
    int sec =  ((up - hour*60*60) - min*60);
    QString e = QString::number(hour) +  QString(" h. ") + QString::number(min) + QString(" m. ")
                + QString::number(sec) + QString(" s.");
    uptime->setText("Uptime: " + e);
    stream.close(); stream.open("/proc/cpuinfo");
    for(int i = 0; i < 16;i++) stream >> str;
    getline(stream,str);
    proc->setText("CPU: " + QString::fromStdString(str));
    for(int i = 0; i< 7; i++)  stream >> str;
    freq->setText("Frequency: " + QString::fromStdString(str) + " MHz");
    cpubar->setValue(getCpuLoad(0.3));
    stream.close(); stream.open("/proc/meminfo");
    stream >> str; stream >> str;
    int num = atoi(str.c_str());
    int percent = num / 100;
    int gb = (num / 1024) / 1024;
    int mb = (num-gb*1024*1024) /1024;
    int kb = (num - (gb*1024*1024+mb*1024));
    if (gb > 0)
       e = QString::number(gb) + QString(" Gb ");
    else
       e = QString("");
    if (mb > 0)
       e += QString::number(mb) + QString(" Mb ");
    if (kb > 0)
       e += QString::number(kb) + QString(" Kb ");
    mem->setText("RAM: " + e);
    int free = 0;
    for (int i = 0 ; i < 3 ; i++) {
        stream >> str; stream >> str; stream >> str;
        free += atoi(str.c_str());
    }
    num -= free;
    gb = num / 1024 / 1024;
    mb = (num - gb*1024*1024) / 1024;
    kb = (num - ((mb*1024) + (gb * 1024 * 1024)));
    if (gb > 0)
       e = QString::number(gb) + QString(" Gb ");
    else
       e = QString("");
    if (mb > 0)
       e += QString::number(mb) + QString(" Mb ");
    if (kb > 0)
       e += QString::number(kb) + QString(" Kb ");
    memload->setText("USING RAM: " + e);
    percent = num / percent; membar->setValue(percent);
}


int Info::getCpuLoad(double dt)
{
    vector<float> stats1 = readCpuStats();
    QProcess::execute("sleep",QStringList() << QString::number(dt));
    vector<float> stats2 = readCpuStats();
    int size1 = stats1.size();
    int size2 = stats2.size();
    if (!size1 || !size2 || size1 != size2) return 2;
    for (int i = 0; i < size1; ++i) stats2[i] -= stats1[i];
    int sum = 1;
    for (int i = 0; i < size1; ++i) sum += stats2[i];
    int load = 100 - (stats2[size2 - 1] * 100 / sum);
    return load;
}


vector<float> Info::readCpuStats()
{
    vector<float> ret;
    ifstream stat_file("/proc/stat");
    if (!stat_file.is_open())
    {
        cout << "Unable to open /proc/stat" << std::endl;
        return ret;
    }
    int val;
    string tmp;
    stat_file >> tmp;
    for (int i = 0; i < 4; ++i)
    {
        stat_file >> val;
        ret.push_back(val);
    }
    stat_file.close();
    return ret;
}
