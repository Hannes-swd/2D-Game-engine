// ═══════════════════════════════════════════════════════════════════════════════
//  player_ext_methods.h  –  Public Methoden der Player-Klasse (Modder-Datei)
//
//  Hier Methoden eintragen – sie werden automatisch public Member der
//  player-Klasse und können von überall mit g_player->Methode() aufgerufen
//  werden.
//
//  Die privaten Variablen aus player_ext_vars.h sind hier direkt zugänglich.
// ═══════════════════════════════════════════════════════════════════════════════

int  GetEssen() const       { return essen; }
void GiebEssen(int menge)   { essen = std::min(100, essen + menge); }
void NimmEssen(int menge)   { essen = std::max(0,   essen - menge); }
