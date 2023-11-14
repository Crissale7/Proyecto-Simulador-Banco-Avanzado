#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <fstream>

class Transaccion {
private:
    std::time_t fecha;
    std::string descripcion;
    double monto;

public:
    Transaccion(const std::string& desc, double m) : fecha(std::time(nullptr)), descripcion(desc), monto(m) {}

    std::string obtenerFecha() const {
        std::tm* localTime = std::localtime(&fecha);
        std::stringstream ss;
        ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string obtenerDescripcion() const { return descripcion; }

    double obtenerMonto() const { return monto; }
};

class CuentaBancaria {
private:
    std::string titular;
    std::string contrasena;
    double saldo;
    bool bloqueada;
    std::vector<Transaccion> historial;

public:
    CuentaBancaria(const std::string& nombre, const std::string& pass, double initialBalance)
        : titular(nombre), contrasena(pass), saldo(initialBalance), bloqueada(false) {}

    void realizarDeposito(double monto) {
        if (monto > 0) {
            saldo += monto;
            registrarTransaccion("Depósito", monto);
        }
        else {
            throw std::invalid_argument("El monto del depósito debe ser mayor que cero.");
        }
    }

    bool realizarRetiro(double monto) {
        if (!bloqueada && monto > 0 && saldo >= monto) {
            saldo -= monto;
            registrarTransaccion("Retiro", -monto);
            return true;
        }
        return false;
    }

    bool realizarTransferencia(CuentaBancaria& destinatario, double monto) {
        if (!bloqueada && monto > 0 && saldo >= monto) {
            saldo -= monto;
            destinatario.saldo += monto;
            registrarTransaccion("Transferencia a " + destinatario.titular, -monto);
            destinatario.registrarTransaccion("Transferencia de " + titular, monto);
            return true;
        }
        return false;
    }

    double obtenerSaldo() const { return saldo; }

    const std::vector<Transaccion>& obtenerHistorial() const { return historial; }

    bool estaBloqueada() const { return bloqueada; }

    bool bloquearCuenta(const std::string& pass) {
        if (!bloqueada && pass == contrasena) {
            bloqueada = true;
            return true;
        }
        return false;
    }

    void desbloquearCuenta() {
        bloqueada = false;
    }

    void cambiarContrasena(const std::string& nuevaPass, const std::string& antiguaPass) {
        if (antiguaPass == contrasena) {
            contrasena = nuevaPass;
        }
        else {
            throw std::invalid_argument("La contraseña antigua no es válida.");
        }
    }

private:
    void registrarTransaccion(const std::string& descripcion, double monto) {
        historial.emplace_back(descripcion, monto);
    }
};

class GestorArchivos {
public:
    static void guardarTicket(const std::string& nombreArchivo, const std::string& contenido) {
        std::ofstream archivo(nombreArchivo);
        if (archivo.is_open()) {
            archivo << contenido;
            archivo.close();
            std::cout << "Ticket guardado exitosamente en " << nombreArchivo << ".\n";
        }
        else {
            std::cerr << "Error al guardar el ticket.\n";
        }
    }
};

void mostrarHistorial(const CuentaBancaria& cuenta) {
    std::cout << "\n--- Historial de Transacciones ---\n";
    for (const Transaccion& transaccion : cuenta.obtenerHistorial()) {
        std::cout << transaccion.obtenerFecha() << ": " << transaccion.obtenerDescripcion() << " - Monto: "
            << transaccion.obtenerMonto() << "\n";
    }
}
void imprimirTicket(const CuentaBancaria& cuenta, const std::string& nombreArchivo) {
    std::stringstream ticket;
    ticket << "------ Ticket Bancario ------\n";
    ticket << "Fecha: " << obtenerFechaActual() << "\n";
    ticket << "Titular: " << cuenta.obtenerTitular() << "\n";
    ticket << "Saldo Actual: " << cuenta.obtenerSaldo() << "\n";
    ticket << "------ Transacciones ------\n";

    for (const Transaccion& transaccion : cuenta.obtenerHistorial()) {
        ticket << transaccion.obtenerFecha() << ": " << transaccion.obtenerDescripcion() << " - Monto: "
            << transaccion.obtenerMonto() << "\n";
    }

    ticket << "------ Fin del Ticket ------";

    std::cout << ticket.str() << "\n";

    GestorArchivos::guardarTicket(nombreArchivo, ticket.str());
}

std::string obtenerFechaActual() {
    std::time_t ahora = std::time(nullptr);
    std::tm* localTime = std::localtime(&ahora);
    std::stringstream ss{};
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    CuentaBancaria cuenta1("Juan Perez", "contrasena_segura", 1000.0);
    CuentaBancaria cuenta2("Maria Gonzalez", "otra_contrasena", 500.0);

    int opcion;
    do {
        std::cout << "\n--- Menú ---\n";
        std::cout << "1. Realizar Depósito\n";
        std::cout << "2. Realizar Retiro\n";
        std::cout << "3. Transferir Fondos\n";
        std::cout << "4. Consultar Saldo\n";
        std::cout << "5. Ver Historial de Transacciones\n";
        std::cout << "6. Cambiar Contraseña\n";
        std::cout << "7. Bloquear/Desbloquear Cuenta\n";
        std::cout << "8. Imprimir Ticket\n";
        std::cout << "0. Salir\n";
        std::cout << "Ingrese su opción: ";
        std::cin >> opcion;

        switch (opcion) {
        case 1: {
            double montoDeposito;
            std::cout << "Ingrese el monto del depósito: ";
            std::cin >> montoDeposito;

            try {
                cuenta1.realizarDeposito(montoDeposito);
                std::cout << "Depósito realizado con éxito. Nuevo saldo: " << cuenta1.obtenerSaldo() << "\n";
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
            break;
        }
        case 2: {
            double montoRetiro;
            std::cout << "Ingrese el monto del retiro: ";
            std::cin >> montoRetiro;

            if (cuenta1.realizarRetiro(montoRetiro)) {
                std::cout << "Retiro realizado con éxito. Nuevo saldo: " << cuenta1.obtenerSaldo() << "\n";
            }
            else {
                std::cout << "No se pudo realizar el retiro. Verifique el saldo o que la cuenta no esté bloqueada.\n";
            }
            break;
        }
        case 3: {
            double montoTransferencia;
            std::cout << "Ingrese el monto de la transferencia: ";
            std::cin >> montoTransferencia;

            if (cuenta1.realizarTransferencia(cuenta2, montoTransferencia)) {
                std::cout << "Transferencia realizada con éxito. Nuevo saldo en cuenta1: " << cuenta1.obtenerSaldo()
                    << ", Nuevo saldo en cuenta2: " << cuenta2.obtenerSaldo() << "\n";
            }
            else {
                std::cout << "No se pudo realizar la transferencia. Verifique el saldo o que la cuenta1 no esté bloqueada.\n";
            }
            break;
        }
        case 4:
            std::cout << "Saldo actual en cuenta1: " << cuenta1.obtenerSaldo() << "\n";
            break;
        case 5:
            mostrarHistorial(cuenta1);
            break;
        case 6: {
            std::string nuevaContrasena;
            std::cout << "Ingrese la nueva contraseña: ";
            std::cin >> nuevaContrasena;

            try {
                cuenta1.cambiarContrasena(nuevaContrasena, "contrasena_segura");
                std::cout << "Contraseña cambiada con éxito.\n";
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
            break;
        }
        case 7: {
            std::string pass;
            std::cout << "Ingrese la contraseña actual: ";
            std::cin >> pass;

            if (cuenta1.bloquearCuenta(pass)) {
                std::cout << "Cuenta bloqueada exitosamente.\n";
            }
            else {
                std::cout << "No se pudo bloquear la cuenta. Verifique la contraseña.\n";
            }
            break;
        }
        case 8:
            imprimirTicket(cuenta1, "ticket_cuenta1.txt");
            break;
        case 0:
            std::cout << "Saliendo del programa. ¡Hasta luego!\n";
            break;
        default:
            std::cout << "Opción inválida. Inténtelo de nuevo.\n";
        }
    } while (opcion != 0);

    return 0;
}