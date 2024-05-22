i=20;
while(i);
{
        delay(10);
        XIMFunction(17,0,0);
        if(themsg.Command == '\r') break;
        i--;
}
