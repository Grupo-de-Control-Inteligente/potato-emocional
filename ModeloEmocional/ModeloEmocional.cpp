#include "ModeloEmocional.h"


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


bool Comunicacion::HandlerLCM()
{
    // Nos suscribimos a los canales
    handler_lcm = new lcm::LCM("udpm://239.255.67.67:6767?ttl=1");
    if (!handler_lcm->good())
        return false;

    handler_lcm->subscribe("ARD_SENSORES", &Comunicacion::CallbackSensores, this);
    handler_lcm->subscribe("EMOCIONUSUARIO", &Comunicacion::CallbackSpeechAnalyzer, this);

    // Creamos el hilo de lectura
    flag_lcm_running = true;
    return true;
}

void Comunicacion::RecibeLcm()
{
    while (flag_lcm_running)
    {
        if (handler_lcm->handleTimeout(100) < 0) //milisegundos
            flag_lcm_running = false;
    }
}


bool Comunicacion::EnviaLcm()
{

    // Mensaje de estado de los actuadores


    msg_orden_pulso.frecuencia = consigna_pulso;

    msg_orden_mejillas.rojo = consigna_mejillas_r;
    msg_orden_mejillas.verde = consigna_mejillas_g;
    msg_orden_mejillas.azul = consigna_mejillas_b;

    msg_orden_cola.posicion = consigna_cola_servo;
    msg_orden_cola.velocidad = consigna_cola_motor;

    if(msg_orden_cola.velocidad < 20){
    	msg_orden_cola.velocidad = 0;
    }

    if (handler_lcm->publish("ARD_ORDEN_PULSO", &msg_orden_pulso))
        return false;
    if (handler_lcm->publish("ARD_ORDEN_COLA", &msg_orden_cola))
        return false;
    if (handler_lcm->publish("ARD_ORDEN_MEJILLAS", &msg_orden_mejillas))
        return false;

    // Mensaje del estado GENERAL de potato (cara y conversación)

    msg_estado_gen.estado = consigna_estado;

    if (handler_lcm->publish("ESTADO_GEN", &msg_estado_gen))
        return false;


    return true;

}

void Comunicacion::CallbackSensores(const lcm::ReceiveBuffer* buffer_recv_, 
						const std::string& canal_, 
						const potatolcm::ard_sensores* msg_recv_)
{
    //printf("Received message on channel \"%s\":\n", canal_.c_str());
    //cout << msg_recv_->luz_derecha <<"\n" << "AQUI_call" << "\n";
    
    // Lo que nos llega de los sensores
    //printf("\nEntro en CallbackSensores");
    valor_luz_izq = msg_recv_->luz_izquierda;
    valor_luz_der = msg_recv_->luz_derecha;
    
    valor_oreja_izq = msg_recv_->oreja_izquierda;
    valor_oreja_der = msg_recv_->oreja_derecha;
    
    valor_antena = msg_recv_->antena;
    
    valor_diadema = msg_recv_->diadema;

    // Recibir algo de la conversación (?????)

    /*
	*
	*
	*
	*/


    doFuzzy();

}

void Comunicacion::CallbackSpeechAnalyzer(const lcm::ReceiveBuffer* buffer_recv_, 
						const std::string& canal_, 
						const potatolcm::emocion_usuario* msg_recv_){

	valor_emocion = msg_recv_->emocion;
	printf("\nEstado del usuario: %s\n", valor_emocion.c_str());

	doFuzzy();

}

void setLuzAlerta(){

	// MAQUINA

	LuzAlerta = new Engine;
	LuzAlerta->setName("Luz_Alerta");
	LuzAlerta->setDescription("");

	// ENTRADA: LUZ AMBIENTE

	Luz = new InputVariable;
	Luz->setName("Luz");
	Luz->setDescription("");
	Luz->setEnabled(true);
	Luz->setRange(0.000, 1023.000);
	Luz->setLockValueInRange(false);
	Luz->addTerm(new Trapezoid("MOSCURO", 0.000, 0.000, 100.000, 200.000));
	Luz->addTerm(new Triangle("OSCURO", 100.000, 200.000, 400.000));
	Luz->addTerm(new Triangle("NORMAL", 200.000, 400.000, 700.000));
	Luz->addTerm(new Triangle("CLARO", 400.000, 700.000, 900.00));
	Luz->addTerm(new Trapezoid("MCLARO", 700.000, 900.000, 1023.000, 1023.000));
	LuzAlerta->addInputVariable(Luz);

	// ENTRADA: BATERÍA

	Bateria = new InputVariable;
	Bateria->setName("Bateria");
	Bateria->setDescription("");
	Bateria->setEnabled(true);
	Bateria->setRange(0.000, 100.000);
	Bateria->setLockValueInRange(false);
	Bateria->addTerm(new Trapezoid("BAJA", 0.000, 0.000, 20.000, 50.000));
	Bateria->addTerm(new Triangle("MEDIA", 20.000, 50.000, 80.000));
	Bateria->addTerm(new Trapezoid("ALTA", 50.000, 80.000, 100.000, 100.000));
	LuzAlerta->addInputVariable(Bateria);


	// AUXILIAR: Nos permite usar la salida como entrada

	AlertaIn = new InputVariable;
	AlertaIn->setName("AlertaIn");
	AlertaIn->setDescription("");
	AlertaIn->setEnabled(true);
	AlertaIn->setRange(0.000, 100.000);
	AlertaIn->setLockValueInRange(false);
	AlertaIn->addTerm(new Trapezoid("CALMA", 0.000, 0.000, 25.000, 50.000));
	AlertaIn->addTerm(new Triangle("NORMAL", 25.000, 50.000, 75.000));
	AlertaIn->addTerm(new Trapezoid("MIEDO", 50.000, 75.000, 100.000, 100.000));
	LuzAlerta->addInputVariable(AlertaIn);


	// SALIDA: NIVEL DE ALERTA

	Alerta = new OutputVariable;
	Alerta->setName("Alerta");
	Alerta->setDescription("");
	Alerta->setEnabled(true);
	Alerta->setRange(0.000, 100.000);
	Alerta->setLockValueInRange(false);
	Alerta->setAggregation(new Maximum);
	Alerta->setDefuzzifier(new Centroid(200));
	Alerta->setDefaultValue(25.000);
	Alerta->setLockPreviousValue(false);
	Alerta->addTerm(new Trapezoid("CALMA", 0.000, 0.000, 25.000, 50.000));
	Alerta->addTerm(new Triangle("NORMAL", 25.000, 50.000, 75.000));
	Alerta->addTerm(new Trapezoid("MIEDO", 50.000, 75.000, 100.000, 100.000));
	LuzAlerta->addOutputVariable(Alerta);

	// BLOQUE1 DE REGLAS: LUZ-ALERTA

	ruleLuzAlerta = new RuleBlock;
	ruleLuzAlerta->setName("ruleLuzAlerta");
	ruleLuzAlerta->setDescription("");
	ruleLuzAlerta->setEnabled(true);
	ruleLuzAlerta->setConjunction(new Minimum);
	ruleLuzAlerta->setDisjunction(new Maximum);
	ruleLuzAlerta->setImplication(new Minimum);
	ruleLuzAlerta->setActivation(new General);

	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MOSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MOSCURO and AlertaIn is NORMAL then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MOSCURO and AlertaIn is MIEDO then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is OSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is OSCURO and AlertaIn is NORMAL then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is OSCURO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is NORMAL and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is NORMAL and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is NORMAL and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is CLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is CLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is CLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MCLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MCLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is ALTA and Luz is MCLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));

	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MOSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MOSCURO and AlertaIn is NORMAL then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MOSCURO and AlertaIn is MIEDO then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is OSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is OSCURO and AlertaIn is NORMAL then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is OSCURO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is NORMAL and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is NORMAL and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is NORMAL and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is CLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is CLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is CLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MCLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MCLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is MEDIA and Luz is MCLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));

	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MOSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MOSCURO and AlertaIn is NORMAL then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MOSCURO and AlertaIn is MIEDO then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is OSCURO and AlertaIn is CALMA then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is OSCURO and AlertaIn is NORMAL then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is OSCURO and AlertaIn is MIEDO then Alerta is MIEDO", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is NORMAL and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is NORMAL and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is NORMAL and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is CLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is CLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is CLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MCLARO and AlertaIn is CALMA then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MCLARO and AlertaIn is NORMAL then Alerta is CALMA", LuzAlerta));
	ruleLuzAlerta->addRule(Rule::parse("if Bateria is BAJA and Luz is MCLARO and AlertaIn is MIEDO then Alerta is NORMAL", LuzAlerta));

	LuzAlerta->addRuleBlock(ruleLuzAlerta);
}

void setAlertaLatido(){

	// MAQUINA

	AlertaLatido = new Engine;
	AlertaLatido->setName("Alerta_Latido");
	AlertaLatido->setDescription("");

	// Como entrada configuramos AlertaIN (la de LuzAlerta)

	AlertaLatido->addInputVariable(AlertaIn);

	// SALIDA: VELOCIDAD DEL LATIDO

	vLatido = new OutputVariable;
	vLatido->setName("vLatido");
	vLatido->setDescription("");
	vLatido->setEnabled(true);
	vLatido->setRange(30.000, 300.000);
	vLatido->setLockValueInRange(false);
	vLatido->setAggregation(new Maximum);
	vLatido->setDefuzzifier(new Centroid(250));
	vLatido->setDefaultValue(60.000);
	vLatido->setLockPreviousValue(false);
	vLatido->addTerm(new Trapezoid("LENTO", 30.000, 30.000, 50.000, 80.000));
	vLatido->addTerm(new Triangle("NORMAL", 50.000, 80.000, 120.000));
	vLatido->addTerm(new Trapezoid("RAPIDO", 80.000, 120.00, 300.000, 300.000));
	AlertaLatido->addOutputVariable(vLatido);

	// BLOQUE2 DE REGLAS: ALERTA-VLATIDO

	ruleAlertaLatido = new RuleBlock;
	ruleAlertaLatido->setName("ruleAlertaLatido");
	ruleAlertaLatido->setDescription("");
	ruleAlertaLatido->setEnabled(true);
	ruleAlertaLatido->setConjunction(new Minimum);
	ruleAlertaLatido->setDisjunction(new Maximum);
	ruleAlertaLatido->setImplication(new Minimum);
	ruleAlertaLatido->setActivation(new General);
	ruleAlertaLatido->addRule(Rule::parse("if AlertaIn is CALMA then vLatido is LENTO", AlertaLatido));
	ruleAlertaLatido->addRule(Rule::parse("if AlertaIn is NORMAL then vLatido is NORMAL", AlertaLatido));
	ruleAlertaLatido->addRule(Rule::parse("if AlertaIn is MIEDO then vLatido is RAPIDO", AlertaLatido));
	AlertaLatido->addRuleBlock(ruleAlertaLatido);

}

void setCariciaAnimica(){

	// MAQUINA

	CariciaAnimica = new Engine;
	CariciaAnimica->setName("Caricia_Animica");
	CariciaAnimica->setDescription("");

	// ENTRADA: Caricia

	Caricia = new InputVariable;
	Caricia->setName("Caricia");
	Caricia->setDescription("");
	Caricia->setEnabled(true);
	Caricia->setRange(0.000, 3.000);
	Caricia->setLockValueInRange(false);
	Caricia->addTerm(new Rectangle("NADA", 2.500, 3.000));
	Caricia->addTerm(new Rectangle("POCO", 0.500, 2.500));
	Caricia->addTerm(new Rectangle("MUCHO", 0.000, 0.500));
	CariciaAnimica->addInputVariable(Caricia);

	// ENTRADA: BATERIA

	CariciaAnimica->addInputVariable(Bateria);

	// AUXILIAR: Nos permite usar la salida como entrada

	AnimicaIn = new InputVariable;
	AnimicaIn->setName("AnimicaIn");
	AnimicaIn->setDescription("");
	AnimicaIn->setEnabled(true);
	AnimicaIn->setRange(0.000, 120.000);
	AnimicaIn->setLockValueInRange(false);
	AnimicaIn->addTerm(new Trapezoid("MTRISTE", 0.000, 0.000, 15.000, 30.000));
	AnimicaIn->addTerm(new Triangle("TRISTE", 15.000, 30.000, 45.000));
	AnimicaIn->addTerm(new Triangle("NORMAL", 30.000, 45.000, 80.000));
	AnimicaIn->addTerm(new Triangle("ALEGRE", 60.000, 80.000, 100.000));
	AnimicaIn->addTerm(new Trapezoid("MALEGRE", 80.000, 100.000, 120.000, 120.000));
	CariciaAnimica->addInputVariable(AnimicaIn);

	// SALIDA: NIVEL DE ANIMICA

	Animica = new OutputVariable;
	Animica->setName("Animica");
	Animica->setDescription("");
	Animica->setEnabled(true);
	Animica->setRange(0.000, 120.000);
	Animica->setLockValueInRange(false);
	Animica->setAggregation(new Maximum);
	Animica->setDefuzzifier(new Centroid(200));
	Animica->setDefaultValue(45.000);
	Animica->setLockPreviousValue(false);
	Animica->addTerm(new Trapezoid("MTRISTE", 0.000, 0.000, 15.000, 30.000));
	Animica->addTerm(new Triangle("TRISTE", 15.000, 30.000, 45.000));
	Animica->addTerm(new Triangle("NORMAL", 30.000, 45.000, 80.000));
	Animica->addTerm(new Triangle("ALEGRE", 60.000, 80.000, 100.000));
	Animica->addTerm(new Trapezoid("MALEGRE", 80.000, 100.000, 120.000, 120.000));
	CariciaAnimica->addOutputVariable(Animica);

	// BLOQUE1 DE REGLAS: LUZ-ALERTA

	ruleCariciaAnimica = new RuleBlock;
	ruleCariciaAnimica->setName("ruleCariciaAnimica");
	ruleCariciaAnimica->setDescription("");
	ruleCariciaAnimica->setEnabled(true);
	ruleCariciaAnimica->setConjunction(new Minimum);
	ruleCariciaAnimica->setDisjunction(new Maximum);
	ruleCariciaAnimica->setImplication(new Minimum);
	ruleCariciaAnimica->setActivation(new General);

// Actualizacion introducida en TFG: Ampliacion del modelo emocional de un robot de asistencia personal. Se eliminan las normas Caricia-Animica cuando Caricia = NADA y se introduce la influencia del Speech del Usuario cuando Caricia = NADA

//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is NADA and AnimicaIn is MTRISTE then Animica is MTRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is NADA and AnimicaIn is TRISTE then Animica is TRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is NADA and AnimicaIn is NORMAL then Animica is NORMAL", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is NADA and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is NADA and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is POCO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is POCO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is POCO and AnimicaIn is NORMAL then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is POCO and AnimicaIn is ALEGRE then Animica is MALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is POCO and AnimicaIn is MALEGRE then Animica is MALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is MUCHO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is MUCHO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is MUCHO and AnimicaIn is NORMAL then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is MUCHO and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is ALTA and Caricia is MUCHO and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));

//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is NADA and AnimicaIn is MTRISTE then Animica is MTRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is NADA and AnimicaIn is TRISTE then Animica is TRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is NADA and AnimicaIn is NORMAL then Animica is NORMAL", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is NADA and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is NADA and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is POCO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is POCO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is POCO and AnimicaIn is NORMAL then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is POCO and AnimicaIn is ALEGRE then Animica is MALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is POCO and AnimicaIn is MALEGRE then Animica is MALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is MUCHO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is MUCHO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is MUCHO and AnimicaIn is NORMAL then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is MUCHO and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is MEDIA and Caricia is MUCHO and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));

//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is NADA and AnimicaIn is MTRISTE then Animica is MTRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is NADA and AnimicaIn is TRISTE then Animica is TRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is NADA and AnimicaIn is NORMAL then Animica is TRISTE", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is NADA and AnimicaIn is ALEGRE then Animica is NORMAL", CariciaAnimica));
//	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is NADA and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is POCO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is POCO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is POCO and AnimicaIn is NORMAL then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is POCO and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is POCO and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is MUCHO and AnimicaIn is MTRISTE then Animica is TRISTE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is MUCHO and AnimicaIn is TRISTE then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is MUCHO and AnimicaIn is NORMAL then Animica is NORMAL", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is MUCHO and AnimicaIn is ALEGRE then Animica is ALEGRE", CariciaAnimica));
	ruleCariciaAnimica->addRule(Rule::parse("if Bateria is BAJA and Caricia is MUCHO and AnimicaIn is MALEGRE then Animica is ALEGRE", CariciaAnimica));

	CariciaAnimica->addRuleBlock(ruleCariciaAnimica);

}



void setAnimicaColaVel(){

	//MAQUINA

	AnimicaColaVel = new Engine;
	AnimicaColaVel->setName("Alerta_Latido");
	AnimicaColaVel->setDescription("");


	// Como entrada configuramos AnimicaIn (la de CariciaAlerta)

	AnimicaColaVel->addInputVariable(AnimicaIn);

	// SALIDA: VELOCIDAD DE LA COLA

	ColaVel = new OutputVariable;
	ColaVel->setName("ColaVel");
	ColaVel->setDescription("");
	ColaVel->setEnabled(true);
	ColaVel->setRange(0.000, 120.000);
	ColaVel->setLockValueInRange(false);
	ColaVel->setAggregation(new Maximum);
	ColaVel->setDefuzzifier(new Centroid(250));
	ColaVel->setDefaultValue(60.000);
	ColaVel->setLockPreviousValue(false);
	ColaVel->addTerm(new Trapezoid("MLENTO", 0.000, 0.000, 10.000, 20.000));
//	ColaVel->addTerm(new Triangle("LENTO", 1.000, 2.000, 5.000));
//	ColaVel->addTerm(new Triangle("NORMAL", 2.000, 5.000, 10.000));
//	ColaVel->addTerm(new Triangle("RAPIDO", 5.000, 10.000, 90.000));
	ColaVel->addTerm(new Trapezoid("MRAPIDO", 10.000, 60.000, 120.000, 120.000));
	AnimicaColaVel->addOutputVariable(ColaVel);

	ruleAnimicaColaVel = new RuleBlock;
	ruleAnimicaColaVel->setName("ruleAnimicaColaVel");
	ruleAnimicaColaVel->setDescription("");
	ruleAnimicaColaVel->setEnabled(true);
	ruleAnimicaColaVel->setConjunction(new Minimum);
	ruleAnimicaColaVel->setDisjunction(new Maximum);
	ruleAnimicaColaVel->setImplication(new Minimum);
	ruleAnimicaColaVel->setActivation(new General);
	ruleAnimicaColaVel->addRule(Rule::parse("if AnimicaIn is MTRISTE then ColaVel is MLENTO", AnimicaColaVel));
	ruleAnimicaColaVel->addRule(Rule::parse("if AnimicaIn is TRISTE then ColaVel is MLENTO", AnimicaColaVel));
	ruleAnimicaColaVel->addRule(Rule::parse("if AnimicaIn is NORMAL then ColaVel is MLENTO", AnimicaColaVel));
	ruleAnimicaColaVel->addRule(Rule::parse("if AnimicaIn is ALEGRE then ColaVel is MLENTO", AnimicaColaVel));
	ruleAnimicaColaVel->addRule(Rule::parse("if AnimicaIn is MALEGRE then ColaVel is MRAPIDO", AnimicaColaVel));
	AnimicaColaVel->addRuleBlock(ruleAnimicaColaVel);

}

void setAlertaAnimica(){

	//MAQUINA

	AlertaAnimica = new Engine;
	AlertaAnimica->setName("Alerta_Animica");
	AlertaAnimica->setDescription("");

	// Como entrada configuramos AlertaIn (la de LuzAlerta)

	AlertaAnimica->addInputVariable(AlertaIn);

	// Como entrada configuramos AnimicaIn (la de CariciaAlerta)

	AlertaAnimica->addInputVariable(AnimicaIn);

	// SALIDA: ESTADO GENERAL DEL ROBOT

	EstadoGen = new OutputVariable;
	EstadoGen->setName("EstadoGen");
	EstadoGen->setDescription("");
	EstadoGen->setEnabled(true);
	EstadoGen->setRange(0.000, 100.000);
	EstadoGen->setLockValueInRange(false);
	EstadoGen->setAggregation(new Maximum);
	EstadoGen->setDefuzzifier(new Centroid(250));
	EstadoGen->setDefaultValue(60.000);
	EstadoGen->setLockPreviousValue(false);
	EstadoGen->addTerm(new Trapezoid("MIEDO", 0.000, 0.000, 10.000, 20.000));
	EstadoGen->addTerm(new Triangle("NORMAL", 20.000, 40.000, 60.000));
	EstadoGen->addTerm(new Triangle("ALEGRE", 40.000, 60.000, 80.000));
	EstadoGen->addTerm(new Trapezoid("ALEGRECAL", 60.000, 80.000, 100.000, 100.000));
	AlertaAnimica->addOutputVariable(EstadoGen);

	// BLOQUE1 DE REGLAS: LUZ-ALERTA

	ruleAlertaAnimica = new RuleBlock;
	ruleAlertaAnimica->setName("ruleCariciaAnimica");
	ruleAlertaAnimica->setDescription("");
	ruleAlertaAnimica->setEnabled(true);
	ruleAlertaAnimica->setConjunction(new Minimum);
	ruleAlertaAnimica->setDisjunction(new Maximum);
	ruleAlertaAnimica->setImplication(new Minimum);
	ruleAlertaAnimica->setActivation(new General);

	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is CALMA and AnimicaIn is MTRISTE then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is CALMA and AnimicaIn is TRISTE then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is CALMA and AnimicaIn is NORMAL then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is CALMA and AnimicaIn is ALEGRE then EstadoGen is ALEGRE", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is CALMA and AnimicaIn is MALEGRE then EstadoGen is ALEGRECAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is NORMAL and AnimicaIn is MTRISTE then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is NORMAL and AnimicaIn is TRISTE then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is NORMAL and AnimicaIn is NORMAL then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is NORMAL and AnimicaIn is ALEGRE then EstadoGen is NORMAL", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is NORMAL and AnimicaIn is MALEGRE then EstadoGen is ALEGRE", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is MIEDO and AnimicaIn is MTRISTE then EstadoGen is MIEDO", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is MIEDO and AnimicaIn is TRISTE then EstadoGen is MIEDO", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is MIEDO and AnimicaIn is NORMAL then EstadoGen is MIEDO", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is MIEDO and AnimicaIn is ALEGRE then EstadoGen is MIEDO", AlertaAnimica));
	ruleAlertaAnimica->addRule(Rule::parse("if AlertaIn is MIEDO and AnimicaIn is MALEGRE then EstadoGen is NORMAL", AlertaAnimica));

	AlertaAnimica->addRuleBlock(ruleAlertaAnimica);
}

void setup(int argc, char** argv){

	FL_IUNUSED(argc);
	FL_IUNUSED(argv);



	// El orden de estos sets es IMPORTANTE

	setLuzAlerta();
	setAlertaLatido();
	setCariciaAnimica();
	setAnimicaColaVel();
	setAlertaAnimica();


	i=false;

	consigna_mejillas_r=0;
	consigna_mejillas_g=0;
	consigna_mejillas_b=0;
	consigna_cola_servo=50;
	consigna_cola_motor=0;
	consigna_pulso=50;

	valor_oreja_izq=0;
	valor_oreja_der=0;
	valor_luz_izq=0;
	valor_luz_der=0;
	valor_antena=0;
	valor_diadema=0;

	bat = NIV_BAT;
	estado_MaquinaEstados = neutral0;

	
}

void retardo(InputVariable* In, OutputVariable* Out, float cte){

	// DINÁMICA

	In->setValue((cte*In->getValue()+Out->getValue())/(cte+1));
}

void retardoAlerta(){

	retardo(AlertaIn, Alerta, CTE_ALERTA);
}


void retardoAnimica(){

	retardo(AnimicaIn, Animica, CTE_ANIMICA);
}



void maquinaEstados(){	// Bateria media y alta

	//cout << "Estado previo: " << estado_MaquinaEstados << "\n";

	if((Bateria->getValue())>=30.0){
		if(!strcmp(valor_emocion.c_str(),"happy")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral5_1;
			else if(estado_MaquinaEstados == neutral1_1)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral2_1)	estado_MaquinaEstados = neutral6_1;
			else if(estado_MaquinaEstados == neutral3_1)	estado_MaquinaEstados = neutral1_1;
			else if(estado_MaquinaEstados == neutral4_1)	estado_MaquinaEstados = happy2_1;
			else if(estado_MaquinaEstados == neutral5_1)	estado_MaquinaEstados = neutral7_1;
			else if(estado_MaquinaEstados == neutral6_1)	estado_MaquinaEstados = neutral7_1;
			else if(estado_MaquinaEstados == neutral7_1)	estado_MaquinaEstados = happy1_1;
			else if(estado_MaquinaEstados == neutral8_1)	estado_MaquinaEstados = neutral5_1;
			else if(estado_MaquinaEstados == happy1_1)		estado_MaquinaEstados = happy2_1;
			else if(estado_MaquinaEstados == happy2_1)		estado_MaquinaEstados = happy3_1;
			else if(estado_MaquinaEstados == happy3_1)		estado_MaquinaEstados = veryHappy1_1;
			else if(estado_MaquinaEstados == veryHappy1_1)	estado_MaquinaEstados = veryHappy2_1;
			else if(estado_MaquinaEstados == veryHappy2_1)	estado_MaquinaEstados = veryHappy2_1;
			else if(estado_MaquinaEstados == sad1_1)		estado_MaquinaEstados = neutral6_1;
			else if(estado_MaquinaEstados == sad2_1)		estado_MaquinaEstados = neutral6_1;
			else if(estado_MaquinaEstados == sad3_1)		estado_MaquinaEstados = sad2_1;
			else if(estado_MaquinaEstados == verySad1_1)	estado_MaquinaEstados = sad2_1;
			else if(estado_MaquinaEstados == verySad2_1)	estado_MaquinaEstados = verySad1_1;
			else estado_MaquinaEstados = neutral0;
		}

		if(!strcmp(valor_emocion.c_str(),"neutral")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral0; 
			else if(estado_MaquinaEstados == neutral1_1)	estado_MaquinaEstados = neutral2_1;
			else if(estado_MaquinaEstados == neutral2_1)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral3_1)	estado_MaquinaEstados = neutral2_1;
			else if(estado_MaquinaEstados == neutral4_1)	estado_MaquinaEstados = neutral7_1;
			else if(estado_MaquinaEstados == neutral5_1)	estado_MaquinaEstados = neutral8_1;
			else if(estado_MaquinaEstados == neutral6_1)	estado_MaquinaEstados = neutral3_1;
			else if(estado_MaquinaEstados == neutral7_1)	estado_MaquinaEstados = neutral8_1;
			else if(estado_MaquinaEstados == neutral8_1)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == happy1_1)		estado_MaquinaEstados = neutral7_1;
			else if(estado_MaquinaEstados == happy2_1)		estado_MaquinaEstados = happy1_1;
			else if(estado_MaquinaEstados == happy3_1)		estado_MaquinaEstados = happy2_1;
			else if(estado_MaquinaEstados == veryHappy1_1)	estado_MaquinaEstados = happy3_1;
			else if(estado_MaquinaEstados == veryHappy2_1)	estado_MaquinaEstados = veryHappy1_1;
			else if(estado_MaquinaEstados == sad1_1)		estado_MaquinaEstados = neutral3_1;
			else if(estado_MaquinaEstados == sad2_1)		estado_MaquinaEstados = sad1_1;
			else if(estado_MaquinaEstados == sad3_1)		estado_MaquinaEstados = sad2_1;
			else if(estado_MaquinaEstados == verySad1_1)	estado_MaquinaEstados = sad3_1;
			else if(estado_MaquinaEstados == verySad2_1)	estado_MaquinaEstados = verySad1_1;
			else estado_MaquinaEstados = neutral0;
		}

		if(!strcmp(valor_emocion.c_str(),"sad")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral1_1; 
			else if(estado_MaquinaEstados == neutral1_1)	estado_MaquinaEstados = neutral3_1;
			else if(estado_MaquinaEstados == neutral2_1)	estado_MaquinaEstados = neutral1_1;
			else if(estado_MaquinaEstados == neutral3_1)	estado_MaquinaEstados = sad1_1;
			else if(estado_MaquinaEstados == neutral4_1)	estado_MaquinaEstados = neutral3_1;
			else if(estado_MaquinaEstados == neutral5_1)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral6_1)	estado_MaquinaEstados = sad2_1;
			else if(estado_MaquinaEstados == neutral7_1)	estado_MaquinaEstados = neutral5_1;
			else if(estado_MaquinaEstados == neutral8_1)	estado_MaquinaEstados = neutral4_1;
			else if(estado_MaquinaEstados == happy1_1)		estado_MaquinaEstados = neutral4_1;
			else if(estado_MaquinaEstados == happy2_1)		estado_MaquinaEstados = neutral4_1;
			else if(estado_MaquinaEstados == happy3_1)		estado_MaquinaEstados = happy2_1;
			else if(estado_MaquinaEstados == veryHappy1_1)	estado_MaquinaEstados = happy2_1;
			else if(estado_MaquinaEstados == veryHappy2_1)	estado_MaquinaEstados = veryHappy1_1;
			else if(estado_MaquinaEstados == sad1_1)		estado_MaquinaEstados = sad2_1;
			else if(estado_MaquinaEstados == sad2_1)		estado_MaquinaEstados = sad3_1;
			else if(estado_MaquinaEstados == sad3_1)		estado_MaquinaEstados = verySad1_1;
			else if(estado_MaquinaEstados == verySad1_1)	estado_MaquinaEstados = verySad2_1;
			else if(estado_MaquinaEstados == verySad2_1)	estado_MaquinaEstados = verySad2_1;
			else estado_MaquinaEstados = neutral0;
		}
	}

	else{	// Bateria baja

		if(!strcmp(valor_emocion.c_str(),"happy")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral3_2;
			else if(estado_MaquinaEstados == neutral1_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral2_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral3_2)	estado_MaquinaEstados = neutral4_2;
			else if(estado_MaquinaEstados == neutral4_2)	estado_MaquinaEstados = neutral5_2;
			else if(estado_MaquinaEstados == neutral5_2)	estado_MaquinaEstados = happy1_2;
			else if(estado_MaquinaEstados == happy1_2)		estado_MaquinaEstados = happy1_2;
			else if(estado_MaquinaEstados == happy2_2)		estado_MaquinaEstados = happy1_2;
			else if(estado_MaquinaEstados == sad1_2)		estado_MaquinaEstados = neutral2_2;
			else if(estado_MaquinaEstados == sad2_2)		estado_MaquinaEstados = neutral2_2;
			else if(estado_MaquinaEstados == verySad1_2)	estado_MaquinaEstados = sad2_2;
			else if(estado_MaquinaEstados == verySad2_2)	estado_MaquinaEstados = verySad1_2;
			else estado_MaquinaEstados = neutral0;
		}

		if(!strcmp(valor_emocion.c_str(),"neutral")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral1_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral2_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral3_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral4_2)	estado_MaquinaEstados = neutral3_2;
			else if(estado_MaquinaEstados == neutral5_2)	estado_MaquinaEstados = neutral4_2;
			else if(estado_MaquinaEstados == happy1_2)		estado_MaquinaEstados = happy2_2;
			else if(estado_MaquinaEstados == happy2_2)		estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == sad1_2)		estado_MaquinaEstados = neutral1_2;
			else if(estado_MaquinaEstados == sad2_2)		estado_MaquinaEstados = sad1_2;
			else if(estado_MaquinaEstados == verySad1_2)	estado_MaquinaEstados = sad2_2;
			else if(estado_MaquinaEstados == verySad2_2)	estado_MaquinaEstados = verySad1_2;
			else estado_MaquinaEstados = neutral0;
		}

		if(!strcmp(valor_emocion.c_str(),"sad")){
			if(estado_MaquinaEstados == neutral0) 			estado_MaquinaEstados = neutral1_2;
			else if(estado_MaquinaEstados == neutral1_2)	estado_MaquinaEstados = sad1_2;
			else if(estado_MaquinaEstados == neutral2_2)	estado_MaquinaEstados = sad2_2;
			else if(estado_MaquinaEstados == neutral3_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral4_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == neutral5_2)	estado_MaquinaEstados = neutral0;
			else if(estado_MaquinaEstados == happy1_2)		estado_MaquinaEstados = happy2_2;
			else if(estado_MaquinaEstados == happy2_2)		estado_MaquinaEstados = neutral1_2;
			else if(estado_MaquinaEstados == sad1_2)		estado_MaquinaEstados = sad2_2;
			else if(estado_MaquinaEstados == sad2_2)		estado_MaquinaEstados = verySad1_2;
			else if(estado_MaquinaEstados == verySad1_2)	estado_MaquinaEstados = verySad2_2;
			else if(estado_MaquinaEstados == verySad2_2)	estado_MaquinaEstados = verySad2_2;
			else estado_MaquinaEstados = neutral0;
		}

	}

	//cout << "Nuevo estado: " << estado_MaquinaEstados << "\n";
}

void estadoMaquina_animica(){

	if(estado_MaquinaEstados <= 13) 		Animica->setValue(45.0);	// NORMAL
	else if(estado_MaquinaEstados <= 18) 	Animica->setValue(80.0);	// ALEGRE
	else if(estado_MaquinaEstados <= 20) 	Animica->setValue(80.0); 	// MUY ALEGRE -- Debe ser 100, pero mueve la cola
	else if(estado_MaquinaEstados <= 25) 	Animica->setValue(30.0);	// TRISTE
	else if(estado_MaquinaEstados <= 29) 	Animica->setValue(15.0);	// MUY TRISTE

}


void doFuzzy(){
	// estado_MaquinaEstados = neutral1_1;
	// cout << estado_MaquinaEstados << "\n";



	//printf("Entro en doFuzzy");
	Luz->setValue((valor_luz_izq + valor_luz_der)/2);
	//printf("%lf\n", Luz->getValue());

	//bat += valor_oreja_der;

	bat = 80;

	if(bat > 100){
		bat = 100;	
	} else if (bat < 0){ 
		bat = 0;
	}
	
	Bateria->setValue(bat);

	Caricia->setValue(valor_antena);

	//printf("%lf\t%lf\t", Luz->getValue(), Caricia->getValue());
	
		if(!i){
			AlertaIn->setValue(Alerta->getDefaultValue());
			AnimicaIn->setValue(Animica->getDefaultValue());
			i=true;
		}
		else {
			retardoAlerta();
			retardoAnimica();
			
			//printf("%lf\t%lf\t%lf\t%lf", AlertaIn->getValue(), Alerta->getValue(), AnimicaIn->getValue(), Animica->getValue());
			
		}

		LuzAlerta->process();
		AlertaLatido->process();
		consigna_pulso = vLatido->getValue();

		if(strcmp(valor_emocion.c_str(),"")) maquinaEstados(); // Si detecto que el SpeechAnalyzer me envia algo, me voy a la funcion de la maquina de estados para dar un salto
		valor_emocion = ""; // Anulo el valor de la emocion que viene de Speech Analyzer una vez ya se ha registrado

		if (valor_antena != 3.0){
			estado_MaquinaEstados = veryHappy1_1;	// Se cambia el valor de la maquina de estados para poder partir luego desde el MAlegre
			CariciaAnimica->process();
		}
		else estadoMaquina_animica();
		//printf("Animica: %lf\n", Animica->getValue());

		AnimicaColaVel->process();
		consigna_cola_motor = ColaVel->getValue();
		AlertaAnimica->process();

		//cout << Alerta->fuzzyOutputValue();

		if(EstadoGen->getValue() <= 30){
			consigna_estado = "MIEDO";
		} else if(EstadoGen->getValue() <= 50){
			consigna_estado = "NORMAL";
		} else if(EstadoGen->getValue() <= 70){
			consigna_estado = "ALEGRE";
		} else {
			consigna_estado = "ALEGRECAL";
		}

		//printf("\t%d\t%d\t%lf\n", consigna_pulso, consigna_cola_motor, EstadoGen->getValue());


		//consigna_mejillas_g=(255*consigna_pulso)/200-50;

		//consigna_mejillas_b=255*(120-consigna_cola_motor)/120-50;	

	    if(!comunicacion->EnviaLcm()){
			printf("No se pudo enviar\n");	
	    }  

} 




int main(int argc, char** argv){
	system("clear");
	setup(argc, argv);

	comunicacion = new ::Comunicacion();	

	comunicacion->HandlerLCM();
	
	while(1){
		comunicacion->RecibeLcm();
	}


	return 0;
}
