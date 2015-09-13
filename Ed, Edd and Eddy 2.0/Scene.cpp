#include "stdafx.h"

CScene::CScene(void)
{
	useShaders = false;
}

CScene::~CScene(void)
{

}

void CScene::Initialize(void) {

#pragma region Ustawienia

	// Wlaczenie testu glebokosci.
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING); // W��czenie o�wietlenia
	glShadeModel(GL_SMOOTH); // Wyb�r techniki cieniowania
	glEnable(GL_LIGHT0); // W��czenie 0-go �a �wiat

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//glEnable(GL_NORMALIZE);

	// Ustawiamy komponent ambient naszej sceny - wartosc niezalezna od swiatla (warto zresetowac)
	float gl_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gl_amb);

#pragma endregion

#pragma region Zawarto�� sceny

	Terrain = new CTerrain();
	Terrain->Initialize();

	Character = new CCharacter();
	Character->LoadFiles("Resources\\Characters\\Ed\\Ed.obj", "Resources\\Characters\\Ed\\Edx.bmp");
	Character->Initialize();

#pragma endregion

#pragma region Shadery

	MaterialShader = new CShader();
	MaterialShader->CreateShadingProgram("Shaders\\Material.vert", "Shaders\\Material.frag");

	TextureShader = new CShader();
	TextureShader->CreateShadingProgram("Shaders\\Texture.vert", "Shaders\\Texture.frag");

#pragma endregion
}

void CScene::Update(void) {

#pragma region Ruch kamery

	// Zmiana predkosci gracza jesli wcisniete W/S/A/D
	if (keystate['w']) {
		Player.velM = Player.speed;
	}
	if (keystate['s']) {
		Player.velM = -Player.speed;
	}
	if (keystate['a']) {
		Player.velS = -Player.speed;
	}
	if (keystate['d']) {
		Player.velS = Player.speed;
	}

	// Obrot kamery
	float phi = atan2(Player.dir.z, Player.dir.x);
	if (keystate['q']) {
		phi -= 0.02f * Player.speed;
	}
	if (keystate['e']) {
		phi += 0.02f * Player.speed;
	}

	Player.dir.x = cos(phi);
	Player.dir.z = sin(phi);

	// Znalezienie kierunku prostopad�ego
	vec3 per;
	per.x = -Player.dir.z;
	per.y = 0;
	per.z = Player.dir.x;

	// Chodzenie prz�d/ty�
	Player.pos.x += Player.dir.x * Player.velM * .1f;
	Player.pos.y += Player.dir.y * Player.velM * .1f;
	Player.pos.z += Player.dir.z * Player.velM * .1f;

	// Chodzenie na boki
	Player.pos.x += per.x * Player.velS * .1f;
	Player.pos.z += per.z * Player.velS * .1f;

	// Bezw�adno�� - w ka�dym cyklu maleje pr�dko�� gracza
	Player.velM /= 1.2f;
	Player.velS /= 1.2f;

#pragma endregion

#pragma region Debugowanie
	
	//system("cls");
	//cout << Player.pos.x << endl <<
	//	Player.pos.y << endl <<
	//	Player.pos.z << endl;
#pragma endregion

}

void CScene::Render(void) {

	// Wyczysc zawartosc bufora koloru i glebokosci.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Wybor macierzy, ktora od tej pory bedziemy modyfikowac
	// - macierz Modelu/Widoku.
	glMatrixMode(GL_MODELVIEW);

	// Zaladowanie macierzy jednostkowej.
	glLoadIdentity();

#pragma region Kamera

	gluLookAt(
		Player.pos.x + Player.cam.x, Player.pos.y + Player.cam.y, Player.pos.z + Player.cam.z,
		Player.pos.x + Player.cam.x + Player.dir.x, Player.pos.y + Player.cam.y + Player.dir.y, Player.pos.z + Player.cam.z + Player.dir.z,
		0.0f, 1.0f, 0.0f
	);

#pragma endregion

	if (useShaders) {
		glUseProgram(TextureShader->GetShadingProgram());
		//cout << "Shaders enabled" << endl;
	}
	else {
		glUseProgram(0);
		//cout << "Shaders disabled" << endl;
	}

#pragma region �wiat�o
	// Ostatni komponent pozycji �wiat�a: 0 - �wiat�o kierunkowe, 1 - �wiat�o pozycyjne  
	// Podajemy warto�ci b�d�ce przeciwie�stwem kierunku �wiecenia

	float l0_amb[] = { .2f, .2f, .2f, 1.0f };
	float l0_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float l0_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float l0_pos[] = {0.0f, 2.0f, 2.0f, 1.0f }; 
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);

#pragma endregion

#pragma region Obiekty

	Terrain->Render();

#pragma endregion

#pragma region Debugowanie


	Character->Render();

	static double frame = 0;

	frame += 0.5;

//	glDisable(GL_LIGHTING);

	float m0_amb[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float m0_dif[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float m0_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, m0_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m0_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m0_spe);
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

	glPushMatrix();
		glTranslatef(2.0f, 2.0f, -2.0f);
		glRotatef((GLfloat)frame, 0.0f, 1.0f, 0.0f);
		//glScalef(0.5f, 0.5f, 0.5f);
		glutSolidSphere(1.0f, 10, 10);
	glPopMatrix();

	float m1_amb[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float m1_dif[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float m1_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, m1_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m1_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m1_spe);
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

	glPushMatrix();
		glTranslatef(-2.0f, 2.0f, -2.0f);
		glutSolidSphere(1.0f, 10, 10);
	glPopMatrix();

//	glEnable(GL_LIGHTING);

#pragma endregion

}