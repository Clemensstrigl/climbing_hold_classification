#include <iostream>
#include <cmath>

struct LabColor {
    double L; // L* (Lightness) [0-100]
    double a; // a* (Green-Red) [-128 to 127]
    double b; // b* (Blue-Yellow) [-128 to 127]
};

struct RGBColor {
    int R; // Red [0-255]
    int G; // Green [0-255]
    int B; // Blue [0-255]
};

double F(double t){
    if (t > 0.008856)
        return std::cbrt(t);
    return (double(841) / 108) * t +
            double(4) / 29;
}
// CIELAB to CIE 1931 XYZ conversion
void LabToXYZ(const LabColor& lab, double& X, double& Y, double& Z) {
    double fy = (lab.L + 16.0) / 116.0;
    double fx = lab.a / 500.0 + fy;
    double fz = fy - lab.b / 200.0;

    if (pow(fy, 3.0) > 0.008856) {
        fy = pow(fy, 3.0);
    } else {
        fy = (fy - 16.0 / 116.0) / 7.787;
    }

    if (pow(fx, 3.0) > 0.008856) {
        fx = pow(fx, 3.0);
    } else {
        fx = (fx - 16.0 / 116.0) / 7.787;
    }

    if (pow(fz, 3.0) > 0.008856) {
        fz = pow(fz, 3.0);
    } else {
        fz = (fz - 16.0 / 116.0) / 7.787;
    }

    X = 95.047 * fx;
    Y = 100.000 * fy;
    Z = 108.883 * fz;
}

// CIE 1931 XYZ to sRGB conversion
void XYZToRGB(double X, double Y, double Z, RGBColor& rgb) {
    X /= 100.0;
    Y /= 100.0;
    Z /= 100.0;

    double R = X *  3.2406 + Y * -1.5372 + Z * -0.4986;
    double G = X * -0.9689 + Y *  1.8758 + Z *  0.0415;
    double B = X *  0.0557 + Y * -0.2040 + Z *  1.0570;

    // Apply gamma correction
    R = (R <= 0.0031308) ? 12.92 * R : 1.055 * pow(R, 1.0 / 2.4) - 0.055;
    G = (G <= 0.0031308) ? 12.92 * G : 1.055 * pow(G, 1.0 / 2.4) - 0.055;
    B = (B <= 0.0031308) ? 12.92 * B : 1.055 * pow(B, 1.0 / 2.4) - 0.055;

    // Convert to 8-bit integer values [0-255]
    rgb.R = static_cast<int>(R * 255.0);
    rgb.G = static_cast<int>(G * 255.0);
    rgb.B = static_cast<int>(B * 255.0);

    // Ensure the values are within the valid range [0-255]
    rgb.R = (rgb.R < 0) ? 0 : (rgb.R > 255) ? 255 : rgb.R;
    rgb.G = (rgb.G < 0) ? 0 : (rgb.G > 255) ? 255 : rgb.G;
    rgb.B = (rgb.B < 0) ? 0 : (rgb.B > 255) ? 255 : rgb.B;
}


void lab2rgb(double l, double a, double b, double *R, double *G, double *B){
    double var_Y = ( l + 16.0 ) / 116.0;
    double var_X = a / 500.0 + var_Y;
    double var_Z = var_Y - b / 200.0;

    if ( pow(var_Y,3)  > 0.008856 ) 
        var_Y = pow(var_Y,3);
    else                       
        var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;
    if ( pow(var_X,3)  > 0.008856 ) 
        var_X = pow(var_X,3);
    else                       
        var_X = ( var_X - 16.0 / 116.0 ) / 7.787;
    if ( pow(var_Z,3)  > 0.008856 ) 
        var_Z = pow(var_Z,3);
    else                       
        var_Z = ( var_Z - 16.0 / 116.0 ) / 7.787;
    double Reference_X = 95.047;
    double Reference_Y = 100.0;
    double Reference_Z = 108.883;
    double X = var_X * Reference_X;
    double Y = var_Y * Reference_Y;
    double Z = var_Z * Reference_Z;

    var_X = X / 100;
    var_Y = Y / 100;
    var_Z = Z / 100;

    double var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
    double var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
    double var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

    if ( var_R > 0.0031308 ) var_R = 1.055 * ( pow(var_R , ( 1.0 / 2.4 )) ) - 0.055;
    else                     var_R = 12.92 * var_R;
    if ( var_G > 0.0031308 ) var_G = 1.055 * ( pow(var_G , ( 1.0 / 2.4 )) ) - 0.055;
    else                     var_G = 12.92 * var_G;
    if ( var_B > 0.0031308 ) var_B = 1.055 * ( pow(var_B , ( 1.0 / 2.4 )) ) - 0.055;
    else                     var_B = 12.92 * var_B;

    *R = (double)(var_R * 255);
    *G = (double)(var_G * 255);
    *B = (double)(var_B * 255);
}




double modifyRGBValue(double p){
    if ( p > 0.04045 ) return pow(( ( p + 0.055 ) / 1.055 ) , 2.4);
    else               return p = p / 12.92;
}

void rgb2lab(int R, int G , int B, double *l, double *a, double *b){

    double var_R = modifyRGBValue( (double)R / 255.0 )* 100;
    double var_G = modifyRGBValue( (double)G / 255.0 )* 100;
    double var_B = modifyRGBValue( (double)B / 255.0 )* 100;


    double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
    std::cout << "X: " << X << ", Y: " << Y << ", Z: " << Z << std::endl;
    double Reference_X = 95.047;
    double Reference_Y = 100.0;
    double Reference_Z = 108.883;


    double var_X = X / Reference_X;
    double var_Y = Y / Reference_Y;
    double var_Z = Z / Reference_Z;

    if ( var_X > 0.008856 ) var_X = std::cbrt(var_X);
    else                    var_X = ( 7.787 * var_X ) + ( 16 / 116 );
    if ( var_Y > 0.008856 ) var_Y = std::cbrt(var_Y);
    else                    var_Y = ( 7.787 * var_Y ) + ( 16 / 116 );
    if ( var_Z > 0.008856 ) var_Z = std::cbrt(var_Z);
    else                    var_Z = ( 7.787 * var_Z ) + ( 16 / 116 );

    *l = ( 116 * var_Y ) - 16;
    *a = 500 * ( var_X - var_Y );
    *b = 200 * ( var_Y - var_Z );
    
}

int main() {
    LabColor labColor = {80.0, 20.0, -40.0}; // Example CIELAB color
    double L = 66.887500, A = 67.545655, B = 68.013820;

    double X, Y, Z;
    double r,g,b;
    RGBColor rgbColor;

    // Convert CIELAB to CIE 1931 XYZ
    rgb2lab(L,A,B,&X, &Y, &Z);

    std::cout << "X: " << X << ", Y: " << Y << ", Z: " << Z << std::endl;


    // Convert CIE 1931 XYZ to sRGB
    lab2rgb(L,A,B, &r,&g,&b);

    // Output the BGR color values
    std::cout << "R: " << r << ", G: " << g << ", B: " << b << std::endl;

    return 0;
}
