#include "fl/Headers.h"
#include <lcm/lcm-cpp.hpp>

#include <thread>
#include <math.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <potato/ard_sensores.hpp>
#include <potato/ard_actuadores.hpp>
#include <potato/ard_orden_cola.hpp>
#include <potato/ard_orden_mejillas.hpp>
#include <potato/ard_orden_pulso.hpp>

#include "potatolcm/estado_gen.hpp"

#include "potatolcm/emocion_usuario.hpp"

using namespace fl;
using namespace std;


// CONSTANTES PARA LA DINAMICA

#define CTE_ALERTA 10
#define CTE_ANIMICA 20


// CONSTANTES OTRO

#define NIV_BAT 80

// CLASE PARA COMUNICACION LCM

class Comunicacion
{
public:
    Comunicacion();
    virtual ~Comunicacion();

    void Run();
    bool EnviaLcm();
    bool HandlerLCM();
    void RecibeLcm();

private:
    
    bool flag_lcm_running;
    char buffer_serie_in[128];
    char buffer_serie_out[128];


    // LCM
    std::thread *hilo_lcm;
    lcm::LCM *handler_lcm;
    potatolcm::ard_sensores msg_sensores;
    potatolcm::ard_actuadores msg_actuadores;
    potatolcm::ard_orden_cola msg_orden_cola;
    potatolcm::ard_orden_mejillas msg_orden_mejillas;
    potatolcm::ard_orden_pulso msg_orden_pulso;

    potatolcm::estado_gen msg_estado_gen;

    potatolcm::emocion_usuario msg_emocion_usuario;

    void CallbackSensores(const lcm::ReceiveBuffer* buffer_recv_, const std::string& canal_, const potatolcm::ard_sensores* msg_recv_);
    void CallbackSpeechAnalyzer(const lcm::ReceiveBuffer* buffer_recv_, const std::string& canal_, const potatolcm::emocion_usuario* msg_recv_);
    

   
};

// VARIABLES

// ** Para ver si es la primera vez que se ejecuta el codigo o no
bool i;

// ** Variables  de sensores y actuadores
int valor_oreja_izq, valor_oreja_der, valor_luz_izq, valor_luz_der, valor_antena, valor_diadema;
string valor_emocion;
int bat;
int consigna_mejillas_r, consigna_mejillas_g, consigna_mejillas_b, consigna_cola_servo, consigna_cola_motor, consigna_pulso;
string consigna_estado;

// ** Variables fuzzy
Engine* LuzAlerta;
Engine* AlertaLatido;
Engine* CariciaAnimica;
Engine* AnimicaColaVel;
Engine* AlertaAnimica;

InputVariable* Luz;
InputVariable* Bateria;
InputVariable* AlertaIn;
InputVariable* Caricia;
InputVariable* AnimicaIn;

OutputVariable* Alerta;
OutputVariable* vLatido;
OutputVariable* Animica;
OutputVariable* ColaVel;
OutputVariable* EstadoGen;

RuleBlock* ruleLuzAlerta;
RuleBlock* ruleAlertaLatido;
RuleBlock* ruleCariciaAnimica;
RuleBlock* ruleAnimicaColaVel;
RuleBlock* ruleAlertaAnimica;

scalar* general;

// Variables para la maquina de estados

enum estadosMaquina{
        neutral0 = 0, neutral1_1 = 1, neutral2_1 = 2, neutral3_1 = 3, neutral4_1 = 4, neutral5_1 = 5, neutral6_1 = 6, neutral7_1 = 7, neutral8_1 = 8,
        happy1_1 = 14, happy2_1 = 15, happy3_1 = 16, 
		veryHappy1_1 = 19, veryHappy2_1 = 20,
        sad1_1 = 21, sad2_1 = 22, sad3_1 = 23, 
		verySad1_1 = 26, verySad2_1 = 27,

        neutral1_2 = 9, neutral2_2 = 10, neutral3_2 = 11, neutral4_2 = 12, neutral5_2 = 13, 
        happy1_2 = 17, happy2_2 = 18,
        sad1_2 = 24, sad2_2 = 25,
        verySad1_2 = 28, verySad2_2 = 29
    }estado_MaquinaEstados;


// Funciones

void doFuzzy();
void retardoAlerta();
void retardoAnimica();
void retardo();
void setup(int argc, char** argv);
void setAlertaLatido();
void setLuzAlerta();
void setCariciaAnimica();
void setAnimicaColaVel();

void maquinaEstados();
void estadoMaquina_animica();

Comunicacion *comunicacion;
