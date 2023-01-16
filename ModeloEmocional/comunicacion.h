#include "include/fl/Headers.h"
#include <lcm/lcm-cpp.hpp>

#include <thread>

#include <potato/ard_sensores.hpp>
#include <potato/ard_actuadores.hpp>
#include <potato/ard_orden_cola.hpp>
#include <potato/ard_orden_mejillas.hpp>
#include <potato/ard_orden_pulso.hpp>

class Comunicacion
{
public:
    Comunicacion();
    virtual ~Comunicacion();

    void Run();
    bool EnviaLcm();


private:
    // Variables
    int valor_oreja_izq, valor_oreja_der, valor_luz_izq, valor_luz_der, valor_antena, valor_diadema;

    int consigna_mejillas_r, consigna_mejillas_g, consigna_mejillas_b, consigna_cola_servo, consigna_cola_motor, consigna_pulso;
    int valor_luz;

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

    void CallbackSensores(const lcm::ReceiveBuffer* buffer_recv_, const std::string& canal_, const potatolcm::ard_sensores* msg_recv_);
    bool IniciaHiloLcm();
    void RecibeLcm();

   
};