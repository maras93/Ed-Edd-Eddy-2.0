#pragma once

// Scena - Jest to podstawowy element organizuj�cy hierarchi� obiekt�w w grze.
// Z za�o�enia, w danym momencie powinna istnie� tylko jedna scena.
// W sytuacji gdy scen mo�e by� wi�cej, dobrze jest utworzy� klasy dziedzicz�ce po tej.
class CScene
{
public:
	CTerrain * Terrain;
	CPlayer Player;

	CScene();
	~CScene();
	void Initialize(void);
	void Update(void);
	void Render(void);

private:

};

