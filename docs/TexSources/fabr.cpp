AutoLocation * ConnectToServer::getAutoLocation(int ind)
{
    switch(ind)
    {
        case 0:
            return new RandomLocation;
        case 1:
            return new RandMaxLocation;
        case 2:
            return new EdgeLocation;
    }
    return NULL;
}