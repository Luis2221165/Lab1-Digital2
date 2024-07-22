//***************************************************************************
//Universidad del valle de Guatemala
//IE2023: Programaci?n de Microcontroladores
//Autor: Luis Angel Ramirez Or?zco
//Proyecto: Práctica 1
//Hardware: ATMEGA328P
//***************************************************************************


#include <avr/io.h>
#include <util/delay.h>

// Definición de pines
#define BUTTON_PIN1 PD2 // Pin del primer botón (D2)
#define BUTTON_PIN2 PD3 // Pin del segundo botón (D3)
#define LED_PINS1 (1<<PD4 | 1<<PD5 | 1<<PD6 | 1<<PD7) // Pines de los LEDs para el primer contador (D4-D7)
#define LED_PINS2 (1<<PB0 | 1<<PB1 | 1<<PB2 | 1<<PB3) // Pines de los LEDs para el segundo contador (B0-B3)
#define DISPLAY_PINS (1<<PC0 | 1<<PC1 | 1<<PC2 | 1<<PC3 | 1<<PC4 | 1<<PC5 | 1<<PB4) // Pines del display de 7 segmentos (C0-C5, B4)

// Variables
uint8_t contador1 = 0; // Contador de décadas de 4 bits para el primer botón
uint8_t contador2 = 0; // Contador de décadas de 4 bits para el segundo botón
uint8_t boton1Presionado = 0; // Estado del primer botón
uint8_t boton2Presionado = 0; // Estado del segundo botón
uint8_t cuentaRegresivaTerminada = 0; // Estado de la cuenta regresiva
uint8_t conteoCompletado = 0; // Estado del conteo completado

// Segmentos del display para los números 0-9
const uint8_t segmentos[] = {
	0b0111111, // 0
	0b0000110, // 1
	0b1011011, // 2
	0b1001111, // 3
	0b1100110, // 4
	0b1101101, // 5
	0b1111101, // 6
	0b0000111, // 7
	0b1111111, // 8
	0b1101111  // 9
};

void inicializar() {
	// Configurar pines de los LEDs como salida para el primer contador
	DDRD |= LED_PINS1;
	DDRB |= LED_PINS2;
	
	// Configurar pines del display de 7 segmentos como salida
	DDRC |= 0b00111111; // PC0-PC5
	DDRB |= (1 << PB4); // PB4
	
	// Configurar pines de los botones como entrada con pull-up
	DDRD &= ~((1 << BUTTON_PIN1) | (1 << BUTTON_PIN2));
	PORTD |= (1 << BUTTON_PIN1) | (1 << BUTTON_PIN2);
	
	// Inicializar LEDs apagados para ambos contadores
	PORTD &= ~LED_PINS1;
	PORTB &= ~LED_PINS2;
}

void actualizarLEDs(uint8_t contador, uint8_t ledPort, uint8_t ledPins) {
	// Mostrar el valor del contador en los LEDs
	for (int i = 0; i < 4; ++i) {
		if ((contador >> i) & 1) {
			ledPort |= (1 << i);
			} else {
			ledPort &= ~(1 << i);
		}
	}
}

void actualizarDisplay(uint8_t numero) {
	// Mostrar el número en el display de 7 segmentos
	uint8_t segmentosParaMostrar = segmentos[numero];
	
	for (int i = 0; i < 6; ++i) {
		if ((segmentosParaMostrar >> i) & 1) {
			PORTC |= (1 << i);
			} else {
			PORTC &= ~(1 << i);
		}
	}
	
	if ((segmentosParaMostrar >> 6) & 1) {
		PORTB |= (1 << PB4);
		} else {
		PORTB &= ~(1 << PB4);
	}
}

int main(void) {
	inicializar();
	
	// Realizar la cuenta regresiva
	for (int i = 5; i >= 0; --i) {
		actualizarDisplay(i);
		_delay_ms(1000); // Esperar 1 segundo
	}

	// Marcar que la cuenta regresiva ha terminado
	cuentaRegresivaTerminada = 1;

	// Apagar el display de 7 segmentos
	PORTC &= ~0b00111111; // PC0-PC5
	PORTB &= ~(1 << PB4); // PB4

	while (1) {
		if (!cuentaRegresivaTerminada || conteoCompletado) continue; // No hacer nada si la cuenta regresiva no ha terminado o el conteo ha sido completado

		// Leer el estado del primer botón
		uint8_t boton1Estado = !(PIND & (1 << BUTTON_PIN1)); // Botón activo en bajo
		
		// Detectar flanco de subida del primer botón (botón 1 presionado)
		if (boton1Estado && !boton1Presionado) {
			contador1++;
			if (contador1 > 15) {
				contador1 = 15; // Mantener el contador en 15
				PORTD |= LED_PINS1; // Encender todos los LEDs
				conteoCompletado = 1; // Marcar que el conteo ha sido completado
				actualizarDisplay(1); // Mostrar el número 1 en el display
				} else {
				actualizarLEDs(contador1, PORTD, LED_PINS1); // Actualizar la visualización en LEDs para el primer contador
			}
			boton1Presionado = 1; // Marcar el primer botón como presionado
		}
		
		// Resetear estado del primer botón cuando se suelta
		if (!boton1Estado) {
			boton1Presionado = 0;
		}
		
		// Leer el estado del segundo botón
		uint8_t boton2Estado = !(PIND & (1 << BUTTON_PIN2)); // Botón activo en bajo
		
		// Detectar flanco de subida del segundo botón (botón 2 presionado)
		if (boton2Estado && !boton2Presionado) {
			contador2++;
			if (contador2 > 15) {
				contador2 = 15; // Mantener el contador en 15
				PORTB |= LED_PINS2; // Encender todos los LEDs
				conteoCompletado = 1; // Marcar que el conteo ha sido completado
				actualizarDisplay(2); // Mostrar el número 2 en el display
				} else {
				actualizarLEDs(contador2, PORTB, LED_PINS2); // Actualizar la visualización en LEDs para el segundo contador
			}
			boton2Presionado = 1; // Marcar el segundo botón como presionado
		}
		
		// Resetear estado del segundo botón cuando se suelta
		if (!boton2Estado) {
			boton2Presionado = 0;
		}
	}
}
