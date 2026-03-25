// =============================================================
//  4-AXIS STEPPER DEMO — A4988 / DRV8825 + CNC Shield
//  Librería requerida: AccelStepper
//  Instalá desde el Library Manager: "AccelStepper by Mike McCauley"
// =============================================================
//
//  PINOUT (CNC Shield estándar):
//  ┌────────┬──────┬─────┐
//  │  Eje   │ STEP │ DIR │
//  ├────────┼──────┼─────┤
//  │   X    │   2  │   5 │
//  │   Y    │   3  │   6 │
//  │   Z    │   4  │   7 │
//  │   A    │  12  │  13 │
//  └────────┴──────┴─────┘
//  ENABLE (todos los ejes): Pin 8  → LOW = habilitado
//
//  Si NO usás CNC Shield, cambiá los pines en la sección CONFIG.
// =============================================================

#include <AccelStepper.h>

// ── CONFIG: Pines ────────────────────────────────────────────
#define X_STEP 2
#define X_DIR  5
#define Y_STEP 3
#define Y_DIR  6
#define Z_STEP 4
#define Z_DIR  7
#define A_STEP 12
#define A_DIR  13
#define ENABLE_PIN 8   // LOW = motores habilitados

// ── CONFIG: Parámetros de movimiento ─────────────────────────
#define STEPS_PER_REV   200     // Pasos por vuelta (motor 1.8°)
#define MICROSTEP         8     // Microstepping configurado en el driver
                                //  A4988:  MS1 MS2 MS3 → 1/8 = HIGH LOW HIGH
                                //  DRV8825: M0 M1 M2  → 1/8 = LOW  HIGH LOW
#define STEPS_FULL  (STEPS_PER_REV * MICROSTEP)  // = 1600 pasos = 1 vuelta

#define SPEED_SLOW   400.0   // pasos/seg
#define SPEED_MED   1200.0
#define SPEED_FAST  3200.0
#define ACCEL        800.0   // pasos/seg²

// ── Instancias AccelStepper ───────────────────────────────────
AccelStepper motorX(AccelStepper::DRIVER, X_STEP, X_DIR);
AccelStepper motorY(AccelStepper::DRIVER, Y_STEP, Y_DIR);
AccelStepper motorZ(AccelStepper::DRIVER, Z_STEP, Z_DIR);
AccelStepper motorA(AccelStepper::DRIVER, A_STEP, A_DIR);

// ── Helper: esperar a que todos los motores lleguen ───────────
void waitAll() {
  while (motorX.isRunning() || motorY.isRunning() ||
         motorZ.isRunning() || motorA.isRunning()) {
    motorX.run();
    motorY.run();
    motorZ.run();
    motorA.run();
  }
}

// ── Helper: mover todos con posición absoluta ─────────────────
void moveAllTo(long x, long y, long z, long a) {
  motorX.moveTo(x);
  motorY.moveTo(y);
  motorZ.moveTo(z);
  motorA.moveTo(a);
  waitAll();
}

// ── Helper: imprimir estado por Serial ───────────────────────
void printPos(const char* label) {
  Serial.print(label);
  Serial.print("  X:");  Serial.print(motorX.currentPosition());
  Serial.print("  Y:");  Serial.print(motorY.currentPosition());
  Serial.print("  Z:");  Serial.print(motorZ.currentPosition());
  Serial.print("  A:");  Serial.println(motorA.currentPosition());
}

// ── Secuencias demo ──────────────────────────────────────────

// Demo 1: Cada eje se mueve individualmente de a 1 vuelta ida y vuelta
void demo_individual() {
  Serial.println("\n>>> DEMO 1: Movimiento individual por eje");

  long vuelta = STEPS_FULL;

  motorX.setMaxSpeed(SPEED_MED);
  motorX.moveTo(vuelta);
  while (motorX.isRunning()) motorX.run();
  printPos("X +1 vuelta");
  delay(300);

  motorX.moveTo(0);
  while (motorX.isRunning()) motorX.run();
  printPos("X regresa  ");
  delay(300);

  motorY.setMaxSpeed(SPEED_MED);
  motorY.moveTo(vuelta);
  while (motorY.isRunning()) motorY.run();
  printPos("Y +1 vuelta");
  delay(300);

  motorY.moveTo(0);
  while (motorY.isRunning()) motorY.run();
  printPos("Y regresa  ");
  delay(300);

  motorZ.setMaxSpeed(SPEED_MED);
  motorZ.moveTo(vuelta);
  while (motorZ.isRunning()) motorZ.run();
  printPos("Z +1 vuelta");
  delay(300);

  motorZ.moveTo(0);
  while (motorZ.isRunning()) motorZ.run();
  printPos("Z regresa  ");
  delay(300);

  motorA.setMaxSpeed(SPEED_MED);
  motorA.moveTo(vuelta);
  while (motorA.isRunning()) motorA.run();
  printPos("A +1 vuelta");
  delay(300);

  motorA.moveTo(0);
  while (motorA.isRunning()) motorA.run();
  printPos("A regresa  ");
  delay(500);
}

// Demo 2: Los 4 ejes se mueven simultáneamente en la misma dirección
void demo_simultaneous() {
  Serial.println("\n>>> DEMO 2: Movimiento simultáneo — 4 ejes juntos");

  long pasos = STEPS_FULL * 2;  // 2 vueltas

  motorX.setMaxSpeed(SPEED_FAST);
  motorY.setMaxSpeed(SPEED_FAST);
  motorZ.setMaxSpeed(SPEED_FAST);
  motorA.setMaxSpeed(SPEED_FAST);

  moveAllTo(pasos, pasos, pasos, pasos);
  printPos("Todos avanza");
  delay(400);

  moveAllTo(0, 0, 0, 0);
  printPos("Todos regresa");
  delay(500);
}

// Demo 3: Movimiento en espejo — X e Y opuestos, Z e A opuestos
void demo_mirror() {
  Serial.println("\n>>> DEMO 3: Movimiento en espejo");

  long pasos = STEPS_FULL;

  motorX.setMaxSpeed(SPEED_MED);
  motorY.setMaxSpeed(SPEED_MED);
  motorZ.setMaxSpeed(SPEED_MED);
  motorA.setMaxSpeed(SPEED_MED);

  moveAllTo( pasos, -pasos,  pasos, -pasos);
  printPos("Espejo 1");
  delay(400);

  moveAllTo(-pasos,  pasos, -pasos,  pasos);
  printPos("Espejo 2");
  delay(400);

  moveAllTo(0, 0, 0, 0);
  printPos("Centro   ");
  delay(500);
}

// Demo 4: Secuencia escalonada — cada eje se activa 200ms después del anterior
void demo_wave() {
  Serial.println("\n>>> DEMO 4: Ola / Wave sequence");

  long pasos = STEPS_FULL;

  // Cada eje arranca con un pequeño delay antes de lanzar moveTo
  motorX.moveTo(pasos);
  delay(200);
  motorY.moveTo(pasos);
  delay(200);
  motorZ.moveTo(pasos);
  delay(200);
  motorA.moveTo(pasos);

  waitAll();
  printPos("Ola avanza");
  delay(400);

  motorX.moveTo(0);
  delay(200);
  motorY.moveTo(0);
  delay(200);
  motorZ.moveTo(0);
  delay(200);
  motorA.moveTo(0);

  waitAll();
  printPos("Ola regresa");
  delay(500);
}

// Demo 5: Velocidades diferentes por eje
void demo_speeds() {
  Serial.println("\n>>> DEMO 5: Velocidades distintas por eje");

  long pasos = STEPS_FULL * 3;

  motorX.setMaxSpeed(SPEED_SLOW);
  motorY.setMaxSpeed(SPEED_MED);
  motorZ.setMaxSpeed(SPEED_FAST);
  motorA.setMaxSpeed(SPEED_FAST / 2);

  moveAllTo(pasos, pasos, pasos, pasos);
  printPos("Velocidades dif.");
  delay(400);

  moveAllTo(0, 0, 0, 0);
  printPos("Regresa         ");
  delay(500);
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("=== 4-AXIS STEPPER DEMO iniciando ===");

  // Habilitar drivers (pin EN en LOW)
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);

  // Configurar aceleración y velocidad máxima para cada eje
  motorX.setAcceleration(ACCEL);  motorX.setMaxSpeed(SPEED_MED);
  motorY.setAcceleration(ACCEL);  motorY.setMaxSpeed(SPEED_MED);
  motorZ.setAcceleration(ACCEL);  motorZ.setMaxSpeed(SPEED_MED);
  motorA.setAcceleration(ACCEL);  motorA.setMaxSpeed(SPEED_MED);

  // Posición actual = 0 (home)
  motorX.setCurrentPosition(0);
  motorY.setCurrentPosition(0);
  motorZ.setCurrentPosition(0);
  motorA.setCurrentPosition(0);

  delay(1000);
  Serial.println("Motores listos. Iniciando secuencia demo...");
  delay(500);
}

// ── Loop: ejecuta las demos en ciclo ─────────────────────────
void loop() {
  demo_individual();   delay(800);
  demo_simultaneous(); delay(800);
  demo_mirror();       delay(800);
  demo_wave();         delay(800);
  demo_speeds();       delay(800);

  Serial.println("\n======= CICLO COMPLETO — reiniciando =======\n");
  delay(2000);
}
