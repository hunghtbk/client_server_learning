# SourceDir= /cygdrive/f/Share_dir/WAVE
# OutDir= /cygdrive/f/Share_dir/WAVE/build/bin
ServerIP=10.158.14.149
User=hung.hoang
Pass=Thaibinhmuahe17
ServerDir=/data001/hung.hoang/a35/wave
ServerSourceDir=$ServerDir
ServerScript=server.sh

sync_folder() {
    FolderName=$1
    DestFolderName=$2
    #read -p "Sync $FolderName? [y]\n " -n 1 yn
	yn=1

    case $yn in
        [nN]*)  echo
                echo "Skip sync $FolderName."
                echo;;
            *)  rsync -avz --no-perms --no-owner --no-group --progress --exclude '.git' --rsh="sshpass -p $Pass ssh -l $User" $FolderName $ServerIP:$ServerSourceDir/$DestFolderName
				echo "Finish syncing $FolderName to server.";;        
    esac
}

cd /cygdrive/d/hunght/Wave_TPM4G/3.SourceCode/Virtual_Server
echo "Move to $PWD"

sync_folder rtm-server/. rtm-server/bin/.
sshpass -p $Pass ssh -t $User@$ServerIP "$ServerDir/rtm-server/server.sh"
