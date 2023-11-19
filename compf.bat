@Echo off
x86_64-w64-mingw32-g++ -O3 -static-libgcc -static-libstdc++ main.cpp -o main.exe sovieditorres.res -mwindows -lole32 -luuid -IC:\SFML\include -DSFML_STATIC -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32 
echo d  	
echo d  	
echo d  	
echo Done
echo d 	
echo d 	
echo d 	
main.exe
