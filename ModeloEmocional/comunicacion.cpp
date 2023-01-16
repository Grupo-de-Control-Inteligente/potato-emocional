#include "comunicacion.h"

//using namespace std;
//using namespace fl;



Comunicacion::Comunicacion()
{
    //ctor

    flag_lcm_running = false;
}

Comunicacion::~Comunicacion()
{
    //dtor

    flag_lcm_running = false;
    hilo_lcm->join();
}


void Comunicacion::Run()
{
   
    if (!IniciaHiloLcm())
    {
        printf("Error iniciando el LCM\n¿Está conectado a alguna red?\n");
        return;
    } 

}

bool Comunicacion::IniciaHiloLcm()
{
    // Nos suscribimos a los canales
    handler_lcm = new lcm::LCM("udpm://239.255.67.67:6767?ttl=1");
    if (!handler_lcm->good())
        return false;

    handler_lcm->subscribe("ARD_SENSORES", &Comunicacion::CallbackSensores, this);
    handler_lcm->handle();

    // Creamos el hilo de lectura
    flag_lcm_running = true;
    hilo_lcm = new std::thread(&Comunicacion::RecibeLcm, this);
    return true;
}

void Comunicacion::RecibeLcm()
{
    while (flag_lcm_running)
    {
        if (handler_lcm->handleTimeout(100) < 0) //milisegundos
            flag_lcm_running = false;
    }
    return true;
}


bool Comunicacion::EnviaLcm()
{

    // Mensaje de estado de los actuadores

    msg_actuadores.pulso_frecuencia = consigna_pulso;
    msg_actuadores.cola_posicion = consigna_cola_servo;
    msg_actuadores.cola_velocidad = consigna_cola_motor;
    msg_actuadores.mejillas_rojo = consigna_mejillas_r;
    msg_actuadores.mejillas_verde = consigna_mejillas_g;
    msg_actuadores.mejillas_azul = consigna_mejillas_b;

    msg_orden_pulso.frecuencia = consigna_pulso;

    msg_orden_mejillas.rojo = consigna_mejillas_r;
    msg_orden_mejillas.verde = consigna_mejillas_g;
    msg_orden_mejillas.azul = consigna_mejillas_b;

    msg_orden_cola.posicion = consigna_cola_servo;
    msg_orden_cola.velocidad = consigna_cola_motor;



    if (handler_lcm->publish("ARD_ACTUADORES", &msg_actuadores))
        return false;
    if (handler_lcm->publish("ARD_ORDEN_PULSO", &msg_orden_pulso))
        return false;
    if (handler_lcm->publish("ARD_ORDEN_COLA", &msg_orden_cola))
        return false;
    if (handler_lcm->publish("ARD_ORDEN_MEJILLAS", &msg_orden_mejillas))
        return false;
    //cout << "esto" << msg_actuadores.pulso_frecuencia << "\n";
    return true;

}

void Comunicacion::CallbackSensores(const lcm::ReceiveBuffer* buffer_recv_, 
						const std::string& canal_, 
						const potatolcm::ard_sensores* msg_recv_){
    //printf("Received message on channel \"%s\":\n", canal_.c_str());
    //cout << msg_recv_->luz_derecha <<"\n" << "AQUI_call" << "\n";
    valor_luz_izq = msg_recv_->luz_izquierda;
    valor_luz_der = msg_recv_->luz_derecha;
    valor_oreja_izq = msg_recv_->oreja_izquierda;
    valor_oreja_der = msg_recv_->oreja_derecha;
    valor_lantena = msg_recv_->antena;
    valor_diadema = msg_recv_->diadema;

}