#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>

// Posisi dan orientasi kamera
float camX = 0.0f, camY = 5.0f, camZ = 10.0f;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
int windowWidth = 800, windowHeight = 600;
float lastX = windowWidth / 2.0f, lastY = windowHeight / 2.0f; // Posisi terakhir mouse

// Arah kamera
float dirX = 0.0f, dirY = 0.0f, dirZ = -1.0f;

// Keadaan buku
bool bookOpen = false; // Buku tertutup
float bookAngle = 0.0f; // Sudut rotasi buku
bool animating = false; // Status animasi

// Status lampu
bool lightOn = true; // Lampu menyala

// Fungsi untuk inisialisasi OpenGL
// Fungsi untuk inisialisasi OpenGL
// Fungsi untuk inisialisasi OpenGL
// Fungsi untuk inisialisasi OpenGL
void init() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Mengatur warna background
    glEnable(GL_DEPTH_TEST); // Mengaktifkan depth test untuk 3D
    glEnable(GL_LIGHTING); // Mengaktifkan pencahayaan
    glEnable(GL_LIGHT0); // Mengaktifkan lampu 0

    // Mengatur parameter lampu
    GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 0.5f }; // Intensitas ambient cahaya
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Intensitas diffuse cahaya
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Intensitas specular cahaya
    GLfloat lightPosition[] = { 1.8f, 2.5f, -0.5f, 1.0f }; // Posisi lampu tepat di atas meja

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Mengatur spotlight
    GLfloat spotDirection[] = { 0.0f, -1.0f, 0.0f }; // Arah spotlight ke bawah (menuju meja)
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15.0f); // Mengurangi sudut cutoff, fokus lebih sempit
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0f); // Fokus lebih tajam untuk pencahayaan yang lebih terfokus pada meja dan objek di atasnya

    glEnable(GL_COLOR_MATERIAL); // Mengaktifkan pewarnaan material
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

// Fungsi untuk menggambar kubus dengan ukuran tertentu
void drawCube(float x, float y, float z) {
    glBegin(GL_QUADS); // Memulai menggambar dengan bentuk quad (persegi empat)

    // Sisi depan
    glVertex3f(-x, -y, z);
    glVertex3f(x, -y, z);
    glVertex3f(x, y, z);
    glVertex3f(-x, y, z);

    // Sisi belakang
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);

    // Sisi kiri
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, y, -z);

    // Sisi kanan
    glVertex3f(x, -y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, y, z);
    glVertex3f(x, -y, z);

    // Sisi atas
    glVertex3f(-x, y, -z);
    glVertex3f(-x, y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, y, -z);

    // Sisi bawah
    glVertex3f(-x, -y, -z);
    glVertex3f(x, -y, -z);
    glVertex3f(x, -y, z);
    glVertex3f(-x, -y, z);

    glEnd(); // Mengakhiri menggambar
}

// Fungsi untuk menggambar pensil 3D
void drawPencil() {
    GLUquadric* quad = gluNewQuadric(); // Membuat objek quadric

    // Set warna pensil (kuning)
    glColor3f(1.0, 1.0, 0.0);
    
    // Gambar badan pensil
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Mengatur posisi badan pensil
    gluCylinder(quad, 0.05f, 0.05f, 1.0f, 32, 32); // Menggambar silinder (badan pensil)
    glPopMatrix();

    // Set warna ujung pensil (hitam)
    glColor3f(0.0, 0.0, 0.0);
    
    // Gambar ujung pensil
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f); // Mengatur posisi ujung pensil
    gluCylinder(quad, 0.05f, 0.0f, 0.2f, 32, 32); // Menggambar kerucut (ujung pensil)
    glPopMatrix();

    // Set warna penghapus (merah muda)
    glColor3f(1.0, 0.0, 1.0);

    // Gambar penghapus
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.1f); // Mengatur posisi penghapus
    gluCylinder(quad, 0.06f, 0.06f, 0.1f, 32, 32); // Menggambar silinder (penghapus)
    glPopMatrix();

    // Gambar lingkaran dasar penghapus
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.1f); // Mengatur posisi lingkaran dasar penghapus
    gluDisk(quad, 0.0f, 0.06f, 32, 1); // Menggambar lingkaran penghapus
    glPopMatrix();

    gluDeleteQuadric(quad); // Menghapus objek quadric
}

// Fungsi untuk menggambar buku dengan halaman
void drawBook() {
    // Cover belakang
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f); // Warna biru untuk cover belakang
    glTranslatef(-1.0f, 1.7f, 0.0f);
    glScalef(1.0f, 0.1f, 0.7f);
    drawCube(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    // Halaman buku
    int numPages = 10;
    float pageThickness = 0.01f;
    for (int i = 1; i <= numPages; i++) {
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f); // Warna putih untuk halaman
        glTranslatef(-1.0f, 1.8f + (pageThickness * i), 0.0f);
        glScalef(1.0f, pageThickness, 0.7f);
        drawCube(1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }

    // Cover depan
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f); // Warna merah untuk cover depan
    glTranslatef(-1.0f, 1.8f + (pageThickness * (numPages + 1)), 0.0f);
    glTranslatef(0.0f, 0.0f, 0.7f); // Pindahkan titik rotasi ke ujung kiri cover depan
    glRotatef(bookAngle, 1.0f, 0.0f, 0.0f); // Rotasi buku
    glTranslatef(0.0f, 0.1f, -0.7f); // Kembalikan posisi setelah rotasi
    glScalef(1.0f, 0.1f, 0.7f);
    drawCube(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Fungsi untuk menggambar lampu belajar
void drawTableLamp() {
    // Lamp base
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(1.8f, 1.8f, -0.5f); // Pindahkan ke posisi yang diinginkan di atas meja di pinggir pensil
    glScalef(0.3f, 0.1f, 0.3f); // Perbesar skala pangkalan lampu
    glutSolidCube(1.0f);
    glPopMatrix();

    // Lamp pole
    glPushMatrix();
    glTranslatef(1.8f, 2.2f, -0.5f); // Sesuaikan posisi tiang lampu
    glScalef(0.06f, 0.6f, 0.06f); // Perbesar skala tiang lampu
    glutSolidCube(1.0f);
    glPopMatrix();

    // Lamp shade
    glColor3f(0.9f, 0.9f, 0.8f);
    glPushMatrix();
    glTranslatef(1.8f, 2.5f, -0.5f); // Sesuaikan posisi tudung lampu
    glScalef(0.4f, 0.7f, 0.4f); // Perbesar skala tudung lampu
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.0f, 1.0f, 20, 20);
    glPopMatrix();
}

// Fungsi untuk menampilkan objek di layar
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Membersihkan layar dan buffer depth
    glLoadIdentity(); // Reset transformasi

    // Mengatur kamera
    gluLookAt(camX, camY, camZ, 
              camX + dirX, camY + dirY, camZ + dirZ, 
              0.0f, 1.0f, 0.0f);

    // Mengatur pencahayaan
    if (lightOn) {
        glEnable(GL_LIGHT0); // Menyalakan lampu
    } else {
        glDisable(GL_LIGHT0); // Mematikan lampu
    }

    // Menggambar lantai dan dinding dengan pencahayaan dimatikan
    glDisable(GL_LIGHTING); // Menonaktifkan pencahayaan sementara
    glColor3f(0.4f, 0.4f, 0.4f); // Warna lantai
    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f); // Menempatkan lantai sedikit di bawah
    glScalef(10.0f, 0.1f, 10.0f);
    drawCube(1.0f, 0.6f, 0.6f);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.2f); // Warna dinding kanan
    glPushMatrix();
    glTranslatef(10.0f, 2.0f, -0.1f);
    glScalef(0.1f, 4.0f, 10.0f);
    drawCube(1.0f, 0.6f, 0.6);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.2f); // Warna dinding depan
    glPushMatrix();
    glTranslatef(0.1f, 2.0f, -6.0f);
    glScalef(10.0f, 4.0f, 0.1f);
    drawCube(1.0f, 0.6f, 0.6f);
    glPopMatrix();
    glEnable(GL_LIGHTING); // Mengaktifkan kembali pencahayaan

    // Menggambar meja dan objek lainnya yang disinari cahaya
    // Menyala lampu untuk objek di atas meja
    glPushMatrix();
    glColor3f(0.6f, 0.3f, 0.0f); // Warna permukaan meja
    glTranslatef(0.0f, 1.5f, 0.0f);
    glScalef(4.0f, 0.2f, 2.0f);
    drawCube(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    // Kaki meja
    glColor3f(0.4f, 0.2f, 0.0f);

    // Kaki depan kiri
    glPushMatrix();
    glTranslatef(-1.8f, 0.75f, 0.9f);
    glScalef(0.2f, 1.5f, 0.2f);
    drawCube(1.0f, 0.6f, 1.0f);
    glPopMatrix();

    // Kaki depan kanan
    glPushMatrix();
    glTranslatef(1.8f, 0.75f, 0.9f);
    glScalef(0.2f, 1.5f, 0.2f);
    drawCube(1.0f, 0.6f, 1.0f);
    glPopMatrix();

    // Kaki belakang kiri
    glPushMatrix();
    glTranslatef(-1.8f, 0.75f, -0.9f);
    glScalef(0.2f, 1.5f, 0.2f);
    drawCube(1.0f, 0.6f, 1.0f);
    glPopMatrix();

    // Kaki belakang kanan
    glPushMatrix();
    glTranslatef(1.8f, 0.75f, -0.9f);
    glScalef(0.2f, 1.5f, 0.2f);
    drawCube(1.0f, 0.6f, 1.0f);
    glPopMatrix();

    // Menggambar buku di atas meja
    drawBook();

    // Menggambar lampu belajar di atas meja
    drawTableLamp();

    // Menggambar pensil di pinggir buku
    glPushMatrix();
    glTranslatef(1.1f, 1.8f, -0.5f); // Menempatkan pensil di pinggir buku
    drawPencil();
    glPopMatrix();

    glutSwapBuffers(); // Menukar buffer untuk animasi
}

// Fungsi untuk menangani input keyboard
void keyboard(unsigned char key, int x, int y) {
    float speed = 0.5f; // Kecepatan kamera

    switch (key) {
        case 'w': // Maju
            camX += dirX * speed;
            camZ += dirZ * speed;
            break;
        case 's': // Mundur
            camX -= dirX * speed;
            camZ -= dirZ * speed;
            break;
        case 'a': // Gerak ke kiri
            camX += dirZ * speed;
            camZ -= dirX * speed;
            break;
        case 'd': // Gerak ke kanan
            camX -= dirZ * speed;
            camZ += dirX * speed;
            break;
        case 'o': // Membuka atau menutup buku
            animating = true;
            break;
        case 'l': // Menyalakan/mematikan lampu
            lightOn = !lightOn;
            break;
        case 27: // ESC untuk keluar
            exit(0);
            break;
    }
    glutPostRedisplay(); // Meminta ulang tampilan
}

// Fungsi untuk menangani pergerakan mouse
void mouseMotion(int x, int y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xOffset = x - lastX;
    float yOffset = lastY - y; // Dikurangi karena koordinat y terbalik
    lastX = x;
    lastY = y;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // Batasi pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update arah kamera// Update arah kamera
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;
    dirX = cos(radYaw) * cos(radPitch);
    dirY = sin(radPitch);
    dirZ = sin(radYaw) * cos(radPitch);

    // Normalisasi vektor arah
    float length = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    dirX /= length;
    dirY /= length;
    dirZ /= length;

    glutPostRedisplay(); // Meminta ulang tampilan
}

// Fungsi untuk animasi
void update(int value) {
    if (animating) {
        if (bookOpen) {
            bookAngle -= 5.0f;
            if (bookAngle <= 0.0f) {
                bookAngle = 0.0f;
                animating = false;
                bookOpen = false;
            }
        } else {
            bookAngle += 5.0f;
            if (bookAngle >= 180.0f) {
                bookAngle = 180.0f;
                animating = false;
                bookOpen = true;
            }
        }
        glutPostRedisplay(); // Meminta ulang tampilan
    }
    glutTimerFunc(16, update, 0); // Memanggil fungsi update lagi setelah 16ms
}

// Fungsi untuk menangani perubahan ukuran jendela
void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h); // Menyesuaikan viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 100.0f); // Mengatur perspektif
    glMatrixMode(GL_MODELVIEW);
}

// Fungsi utama
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Menggunakan buffer ganda dan depth
    glutInitWindowSize(windowWidth, windowHeight); // Mengatur ukuran jendela
    glutCreateWindow("3D Room with Table and Books"); // Membuat jendela dengan judul

    init(); // Inisialisasi OpenGL

    glutDisplayFunc(display); // Mengatur fungsi tampilan
    glutReshapeFunc(reshape); // Mengatur fungsi reshape
    glutKeyboardFunc(keyboard); // Mengatur fungsi keyboard
    glutPassiveMotionFunc(mouseMotion); // Mengatur fungsi pergerakan mouse
    glutTimerFunc(16, update, 0); // Memulai fungsi update dengan timer

    glutFullScreen(); // Memulai aplikasi dalam mode layar penuh

    glutMainLoop();
    return 0;
}
