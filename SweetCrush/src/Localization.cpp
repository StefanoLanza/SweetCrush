#include "Localization.h"
#include <algorithm>

namespace {

const char* eng[] = {
	"",
	"SWEET CRUSH",
	"SCORE",
	"START",
	"SETTINGS",
	"AUDIO: ON",
	"AUDIO: OFF",
	"ENGLISH",
	"CREDITS",
	"QUIT",
	"GAME OVER",
	"RETRY",
	"CONTINUE",
	"Code by Stefano Lanza",
	"Graphics by gameartguppy.com",
	"Music by Patrick De Arteaga",
	"BACK",
	"Your score is",
	"Your final score is",
	"You reached level",
	"You completed all levels",
	"PAUSE",
	"CONTINUE",
	"RESTART LEVEL",
	"LEAVE MATCH",
	"OK",
	"LEVEL COMPLETE",
	"NEXT",
	"GAME COMPLETE",
	// Boosters
	"MINI BOMB !",
	"Click on it to destroy nearby pastries",
	"BOMB !",
	"Click on it to destroy nearby pastries",
};

const char* esp[] = {
	"",
	"SWEET CRUSH",
	"PUNTUACION",
	"EMPIEZA",
	"CONFIGURACION",
	"AUDIO: ON",
	"AUDIO: OFF",
	"ESPANOL",
	"CREDITOS",
	"ABANDONA",
	"FIN DEL JUEGO",
	"REINTENTA",
	"SIGUE",
	"Codigo por Stefano Lanza",
	"Grafica por gameartguppy.com",
	"Musica por Patrick De Arteaga",
	"VUELVE",
	"Tu punctuacion es",
	"Tu punctuacion final es",
	"", // TODO
	"",
	"PAUSA",
	"SIGUE",
	"REINICIA NIVEL",
	"ABANDONA PARTIDO",
	"VALE",
	"NIVEL COMPLETO",
	"SIGUIENTE",
	"JUEGO COMPLETO",
	// Boosters
	"MINO BOMBA !",
	"",
	"BOMBA !",
	"",
};

const char* ita[] = {
	"",
	"SWEET CRUSH",
	"PUNTEGGIO",
	"GIOCA",
	"IMPOSTAZIONI",
	"AUDIO: ON",
	"AUDIO: OFF",
	"ITALIANO",
	"CREDITI",
	"ESCI",
	"FINE PARTITA",
	"RIPROVA",
	"CONTINUA",
	"Codice di Stefano Lanza",
	"Grafica di gameartguppy.com",
	"Musica di Patrick De Arteaga",
	"INDIETRO",
	"Il tuo punteggio e'",
	"Il tuo punteggio finale e'",
	"Hai raggiunto il livello",
	"Hai completato tutti i livelli",
	"PAUSA",
	"CONTINUA",
	"RINIZIA IL LIVELLO",
	"ESCI",
	"OK",
	"LIVELLO COMPLETATO",
	"CONTINUA",
	"GIOCO COMPLETO",
	// Boosters
	"MINI BOMBA !",
	"",
	"BOMBA !",
	"",
};

const char** curr = nullptr;
Language     currLanguage {};

} // namespace

void SetLanguage(Language language) {
	if (language == Language::english) {
		curr = eng;
		Wind::SetStringTable(eng, std::size(eng));
	}
	else if (language == Language::spanish) {
		curr = esp;
		Wind::SetStringTable(esp, std::size(esp));
	}
	else {
		curr = ita;
		Wind::SetStringTable(ita, std::size(ita));
	}
	currLanguage = language;
}

void SetNextLanguage() {
	if (currLanguage == Language::english) {
		SetLanguage(Language::spanish);
	}
	else if (currLanguage == Language::spanish) {
		SetLanguage(Language::italian);
	}
	else {
		SetLanguage(Language::english);
	}
}

Language GetCurrentLanguage() {
	return currLanguage;
}

const char* GetLocalizedString(GameStringId str) {
	return curr[static_cast<int>(str)];
}
