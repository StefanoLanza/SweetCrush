#include "Localization.h"

#include <algorithm>
#include <iterator> // std::size

namespace {

const char* eng[] = {
	"",
	"SWEET CRUSH",
	"SCORE",
	"START",
	"PLAY",
	"SETTINGS",
	"LANGUAGE",
	"GRAPHICS",
	"AUDIO",
	"MUSIC",
	"SFX",
	"ENGLISH",
	"CREDITS",
	"QUIT",
	"GAME OVER",
	"RETRY",
	"END GAME",
	"Code by Stefano Lanza",
	"Graphics by gameartguppy.com",
	"Music by Patrick De Arteaga",
	"Version 0.1.0",
	"BACK",
	"LEVEL",
	"GOAL",
	"Your score is",
	"Your final score is",
	"You reached level",
	"You completed all levels",
	"PAUSE",
	"CONTINUE",
	"RESTART LEVEL",
	"END GAME",
	"OK",
	"COMPLETE",
	"NEXT",
	"GAME COMPLETE",
	// Boosters
	"ROCKET !",
	"Click on it to destroy a row of sweets",
	"ROCKET !",
	"Click on it to destroy a column of sweets",
	"MINI BOMB !",
	"Click on it to destroy four adjacent sweets",
	"BOMB !",
	"Click on it to destroy nearby sweets",
};

const char* esp[] = {
	"",
	"SWEET CRUSH",
	"PUNTUACION",
	"EMPEZAR",
	"JUEGA",
	"CONFIGURACION",
	"IDIOMA",
	"GRAFICA",
	"AUDIO",
	"MUSICA",
	"SFX",
	"ESPANOL",
	"CREDITOS",
	"SALIR",
	"FIN DEL JUEGO",
	"REINTENTA",
	"SAL DEL JUEGO",
	"Codigo por Stefano Lanza",
	"Grafica por gameartguppy.com",
	"Musica por Patrick De Arteaga",
	"Version 0.1.0",
	"VOLVER",
	"NIVEL",
	"OBJECTIVO",
	"Tu punctuacion es",
	"Tu punctuacion final es",
	"", // TODO
	"",
	"PAUSA",
	"SEGUIR",
	"REINICIAR",
	"ABANDONAR",
	"VALE",
	"COMPLETO",
	"SIGUIENTE",
	"JUEGO COMPLETO",
	// Boosters
	"COHETE !",
	"",
	"COHETE !",
	"",
	"MINI BOMBA !",
	"",
	"BOMBA !",
	"",
};

const char* ita[] = {
	"",
	"SWEET CRUSH",
	"PUNTEGGIO",
	"INIZIA",
	"GIOCA",
	"IMPOSTAZIONI",
	"LINGUA",
	"GRAFICA",
	"AUDIO",
	"MUSICA",
	"SFX",	
	"ITALIANO",
	"CREDITI",
	"ESCI",
	"FINE PARTITA",
	"RIPROVA",
	"ESCI",
	"Codice di Stefano Lanza",
	"Grafica di gameartguppy.com",
	"Musica di Patrick De Arteaga",
	"Versione 0.1.0",
	"LIVELLO",
	"OBBIETTIVO",
	"INDIETRO",
	"Il tuo punteggio e'",
	"Il tuo punteggio finale e'",
	"Hai raggiunto il livello",
	"Hai completato tutti i livelli",
	"PAUSA",
	"CONTINUA",
	"RICOMINCIA",
	"ABBANDONA",
	"OK",
	"COMPLETATO",
	"CONTINUA",
	"GIOCO COMPLETO",
	// Boosters
	"RAZZO !",
	"Cliccaci sopra per distruggere una riga di dolci",
	"RAZZO !",
	"Cliccaci sopra per distruggere una colonna di dolci",
	"MINI BOMBA !",
	"Cliccaci sopra per distruggere quattro dolci adiacenti",
	"BOMBA !",
	"Cliccaci sopra per distruggere i dolci a lui vicini",
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
