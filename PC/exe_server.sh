GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'
DATE=$date

clear

echo -e "\n${BLUE} -- Compiling $(DATE) --${NC}"

#make config=release -j 3
./compile.sh

echo -e "\n${GREEN} -- Starting execution $(DATE) --${NC}"

export LD_LIBRARY_PATH=../Project/common/dependencies/SFML/lib

./../Project/Server/bin/Server