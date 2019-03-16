echo Trying to delete previous version...
sudo rm -f install-update
echo Building...
sudo g++ -pthread -std=c++11 install-update.cpp -o install-update
echo Executing...
echo "******************************************"
./install-update
echo "******************************************"
#sudo rm -f install-update
echo Done.
