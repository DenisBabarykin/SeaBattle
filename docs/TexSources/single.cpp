class Singleton
{
    private:
        Singleton();
        static const int size = 4;
        QImage imgs[size];
    public:
        static QImage & getImageForShip(int level);
};


Singleton::Singleton()
{
    for(int i=0; i< size; ++i)
        imgs[i] = QImage(QObject::tr(":/images/ship%1.png").arg(i+1));

}

QImage & Singleton::getImageForShip(int level)
{
    static Singleton inst;
    return inst.imgs[level-1];
}
