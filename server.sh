SourceDir=/data001/hung.hoang/a35_rtm_nad
BuildDir=$SourceDir/build
ThreadBuild=8

echo "Source dir: " $SourceDir

#read -p "Do you want to build source? [y]\n " -n 1 yn
echo

cd $SourceDir
BuildResult=-1
#yn = 1
#case $yn in 
#    [Nn]*)  echo "Skip build";;
#        *)  #echo "Build thread(s): $ThreadBuild"
            #read -p "Fresh build? y\[n]" -n 1 f
            #echo
            #case $f in
            #    [Yy]*) rm -rf $BuildDir               
            #esac
            echo "Start building app..."
			./build.sh wave module rtm-server
            BuildResult=$?                        
            
            echo " "       
            echo "        ██████╗ ██╗   ██╗██╗██╗     ██████╗ ";
            echo "        ██╔══██╗██║   ██║██║██║     ██╔══██╗";
            echo "        ██████╔╝██║   ██║██║██║     ██║  ██║";
            echo "        ██╔══██╗██║   ██║██║██║     ██║  ██║";
            echo "        ██████╔╝╚██████╔╝██║███████╗██████╔╝";
            echo "        ╚═════╝  ╚═════╝ ╚═╝╚══════╝╚═════╝ ";
            echo "                                            ";#;            
#esac

case $BuildResult in
    -1)exit 0;;
    0)  
        echo "                      ███████╗██╗   ██╗ ██████╗ ██████╗███████╗███████╗███████╗";
        echo "                      ██╔════╝██║   ██║██╔════╝██╔════╝██╔════╝██╔════╝██╔════╝";
        echo "                      ███████╗██║   ██║██║     ██║     █████╗  ███████╗███████╗";
        echo "                      ╚════██║██║   ██║██║     ██║     ██╔══╝  ╚════██║╚════██║";
        echo "                      ███████║╚██████╔╝╚██████╗╚██████╗███████╗███████║███████║";
        echo "                      ╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝╚══════╝╚══════╝╚══════╝";
        echo "                                                                               ";
        exit 0;;
    *)  
        echo "                      ███████╗ █████╗ ██╗██╗     ███████╗██████╗ ";
        echo "                      ██╔════╝██╔══██╗██║██║     ██╔════╝██╔══██╗";
        echo "                      █████╗  ███████║██║██║     █████╗  ██║  ██║";
        echo "                      ██╔══╝  ██╔══██║██║██║     ██╔══╝  ██║  ██║";
        echo "                      ██║     ██║  ██║██║███████╗███████╗██████╔╝";
        echo "                      ╚═╝     ╚═╝  ╚═╝╚═╝╚══════╝╚══════╝╚═════╝ ";
        echo "                                                                 ";                
        exit 1;;    
esac
