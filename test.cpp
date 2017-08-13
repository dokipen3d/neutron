 #include <iostream>
 
 
inline static int flatten3dCoordinatesto1D(int x, int y,
                                                int z, int chunkSize) {
    return (x + chunkSize * (y + chunkSize * z));
}

int main(){


 std::cout << flatten3dCoordinatesto1D(128,128,128,256) << "\n";


}