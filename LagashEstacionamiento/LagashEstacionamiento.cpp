//Leandro Egea Agosto 2018
//Realizado y probado en Microsoft Visual C++ 2010 Express (Windows 10)

//LIBRERIAS
#include"stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<fstream>
//Linea 713 Envio Mail
#define LUGARES 100//lugares que tenga el estacionamiento
using namespace std;

//ESTRUCTURAS
typedef struct {
	int Lugar;//lugar de estacionamiento que se le asigna al auto
	char Patente[8];//la patente puede tener 7 caracteres y un "\0"
	char Entrada[6];//hora entrada hh:mm
	int EntradaSegundos;//hora contada en segundos desde las 00:00
	char Salida[6];
	int SalidaSegundos;
	int DiasGuardado;//se contabiliza el numero de estadias que tuvo el auto
	char Modelo[25];
}COCHE;
typedef struct {
	int PrecioFraccion;
	int PrecioHora;
	int PrecioEstadia;
}PRECIO;
typedef struct{
	int CajaInicial;
	int Efectivo;
	int Debito;
	int Credito;
}CAJA;

//PROTOTIPOS PRINCIPALES
//Funciones que estan directamente en el main como pequeños programas distintos elegidos mediante un menu
void AbrirDia (COCHE *pIngreso, PRECIO *pPesos, CAJA *pDinero, int *pEstacionados, int *pEspacios);//se usa al comenzar el dia
void IngresoDetectado(COCHE *pIngreso, int *pEstacionados, int *pEspacios);
void EgresoDetectado(COCHE *pIngreso, PRECIO *pPesos, CAJA *pDinero, int *pEstacionados, int *pEspacios);
int FacturarEstadia(COCHE *pIngreso, PRECIO *pPesos, CAJA *pDinero);
void PrecioPorDia(PRECIO *pPesos);
int CantidadEstacionados(COCHE *pIngreso);
int EspaciosDisponibles(COCHE *pIngreso);
void CierreDia(PRECIO *pPesos, CAJA *pDinero, int *pEstacionados);

//PROTOTIPOS AUXILIARES
//Funciones que ayudan a las principales a desarrollar su funcion
int Confirmar(void);//confirmar si se accede a la funcion (si o no)
int IngresarHora(char *pHoraVisual);//al entrar y salir autos se pide la hora, la mide en segundos desde que comenzo el dia
void TransformaHora(char HoraAux[3], char MinutosAux[3], char *pHoraVisual);//pone la hora en formato hh:mm
void EditarPrecio(PRECIO *pPesos);//Al abrir el dia se puede editar el precio
int MedioDePago(void);//efectivo, debito o credito
void LeerDatos(COCHE *pIngreso, PRECIO *pPesos);
void GuardadoDatos(COCHE *pIngreso, PRECIO *pPesos);
void Formato(char *pCadena, int Tamanio);
void EnviarMail(CAJA *pDinero, int *pEstacionados, int AbonadoTotal, int Debe, int Haber, int PlusEstadia);

//MAIN
int main(void)
{
	//estructuras
	COCHE Ingreso[LUGARES], *pIngreso;
	PRECIO Pesos, *pPesos;
	CAJA Dinero, *pDinero;

	//variables
	char Choose;
	int i = 0, Confirmacion = 0, Estacionados, Espacios, Exit = 0, *pEstacionados, *pEspacios;
	for(i = 0;i<LUGARES;i++)
		Ingreso[i].Lugar = 0;//inicializo los lugares para evitar errores
	Pesos.PrecioFraccion = 0;//inicializo todas las variables en caso de que el archivo este vacio
	Pesos.PrecioHora = 0;
	Pesos.PrecioEstadia = 0;
	Dinero.Efectivo = 0;//inicializo las que son contadores
	Dinero.Debito = 0;
	Dinero.Credito = 0;

	//apunto los punteros
	pIngreso = Ingreso;
	pPesos = &Pesos;
	pDinero = &Dinero;
	pEstacionados = &Estacionados;
	pEspacios = &Espacios;

	//archivos
	LeerDatos(pIngreso, pPesos);

	//ABRIR DIA
	printf(" ***ESTACIONAMIENTO LAGASH*** \n");
	do
	{
		printf("A-Abrir Dia \n");
		fflush(stdin);
		scanf("%c", &Choose);
		Choose = tolower(Choose);
		if (Choose == 'a')
		{
			Confirmacion = Confirmar();
			if (Confirmacion == 1)
			{
				pIngreso = Ingreso;
				AbrirDia(pIngreso, pPesos, pDinero, pEstacionados, pEspacios);
				printf("\nBIENVENIDX\n");
			}
		}
		else
			printf("Caracter incorrecto, por favor seleccione una opcion correcta\n");
		printf("Pulse una tecla para continuar...\n");
		fflush(stdin);
		getch(); 
		system("cls");	
	}while (Choose != 'a' || Confirmacion == 0);

	//MENU
	do
	{
		system("cls");
		pIngreso = Ingreso;
		printf("***Seleccione la tarea que desea realizar***\n");

		printf("1-Ingreso detectado \n");
		printf("2-Egreso detectado\n");
		printf("A-Autos estacionados\n");
		printf("B-Espacios disponibles\n");
		printf("C-Consulta precios \n");
		printf("D-Terminar dia \n");

		fflush(stdin);
		scanf("%c", &Choose);
		Choose = tolower(Choose);

		switch(Choose)
		{
		case '1':
			Confirmacion = Confirmar();
			if (Confirmacion == 1)
				IngresoDetectado(pIngreso, pEstacionados, pEspacios);//Ingresamos los datos del coche
			break;
		case '2':
			Confirmacion = Confirmar();
			if (Confirmacion == 1)
				EgresoDetectado(pIngreso, pPesos, pDinero,pEstacionados, pEspacios);
			break;
		case 'a':
			system("cls");
			Estacionados = CantidadEstacionados(pIngreso);
			Espacios = LUGARES - Estacionados;
			break;
		case 'b':
			Espacios = EspaciosDisponibles(pIngreso);
			Estacionados = LUGARES - Espacios;
			break;
		case 'c':
			PrecioPorDia(pPesos);
			break;
		case 'd':
			printf("***ALERTA, NO SE PODRAN REALIZAR MAS INGRESOS Y SALIDAS***\n");
			Confirmacion = Confirmar();
			if (Confirmacion == 1)
			{
				CierreDia(pPesos, pDinero, pEstacionados);
				Exit = 1;
			}
			break;
		default:
			printf("Caracter incorrecto, por favor seleccione una opcion correcta\n");
			break;
		}
		printf("Pulse una tecla para continuar\n");
		fflush(stdin);
		getch(); 
	}while (Exit==0);//vario el exit al final del terminar dia

	GuardadoDatos(pIngreso, pPesos);
	return 0;
}

//FUNCIONES PRINCIPALES
void AbrirDia(COCHE *pIngreso, PRECIO *pPesos,CAJA *pDinero, int *pEstacionados, int *pEspacios)
{
	int Confirmacion, i;
	COCHE *pAux;

	pAux = pIngreso; 
	system("cls");
	*pEstacionados = CantidadEstacionados(pAux);
	*pEspacios = LUGARES - *pEstacionados;

	printf("Pulse una tecla para continuar\n");
	fflush(stdin);
	getch(); 

	do
	{
	PrecioPorDia(pPesos);
	printf("Desea editar los precios?\n");
	Confirmacion = Confirmar();
	if (Confirmacion == 1)
		EditarPrecio(pPesos);
	}while(Confirmacion == 1);

	do
	{
	printf("\nIngrese cuanto dinero hay en la caja \n");
	fflush(stdin);
	scanf("%d", &i);
	pDinero->CajaInicial = i;
	Confirmacion = Confirmar();
	}while((!(pDinero->CajaInicial >= 0)) || Confirmacion == 0);
}
void IngresoDetectado(COCHE *pIngreso, int *pEstacionados, int *pEspacios)
{
	int Confirmacion, i = 0, j = LUGARES;

	system("cls");
	do
	{
		if(pIngreso->Lugar==0)
		{
			pIngreso->Lugar=(i+1);//aca se guarda el lugar donde estara el auto ingresado

			printf("Ingrese Patente (Maximo 7 caracteres)\n");//patente que luego se usa para encontrar el auto cuando sale
			fflush(stdin);
			scanf_s("%[^\n]", pIngreso->Patente, 8);
			Formato(pIngreso->Patente, 8);//para dar el formato deseado a la cadena
			
			pIngreso->EntradaSegundos = IngresarHora(pIngreso->Entrada);
			
			printf("Ingrese Modelo (Maximo 25 caracteres)\n");//solo con fines informativos
			fflush(stdin);
			scanf_s("%[^\n]", pIngreso->Modelo, 25);
			
			pIngreso->Salida[0] = '\0';//inicializo el resto de variables
			pIngreso->SalidaSegundos = 0;
			pIngreso->DiasGuardado = 0;

			j=i;//para guardar el lugar del auto
			i=LUGARES;//para que salga del bucle
		}
		i++;
		if(i<LUGARES)
			pIngreso++;
	}while(i<LUGARES);
	
	if(j<LUGARES)
	{
		printf("\n***Los datos seran guardados***\n\n");
		printf("Lugar Estacionamiento: %d \n", pIngreso->Lugar);
		printf("Patente: %s \n", pIngreso->Patente);
		printf("Horario Entrada: %s \n", pIngreso->Entrada);    
		printf("Modelo: %s \n", pIngreso->Modelo);

		Confirmacion = Confirmar();
		if(Confirmacion==1)
		{
			(*pEstacionados)++;
			printf("La cantidad de autos estacionados es: %d\n", *pEstacionados);
			(*pEspacios)--;
			printf("La cantidad de espacios disponibles es: %d\n", *pEspacios);
		}
		else
		{
			pIngreso->Lugar = 0;
			printf("Los datos no fueron guardados\n");
			printf("La cantidad de autos estacionados es: %d\n", *pEstacionados);
			printf("La cantidad de espacios disponibles es: %d\n", *pEspacios);
		}
	}
	else if(j==LUGARES)
		printf("Ya no quedan espacios disponibles\n");
	else
		printf("Error. Contacte a Sistemas\n");//por precacución
}
void EgresoDetectado(COCHE *pIngreso, PRECIO *pPesos, CAJA *pDinero, int *pEstacionados, int *pEspacios)
{
	int Confirmacion,i, Auto, flag = 0;
	COCHE *pAux;
	do
	{
		pAux = pIngreso;//coloco el puntero auxiliar para utilizarlo con las funciones y poder devolverlo a su lugar inicial cada vez
		system("cls");
		Confirmacion = 0;
		printf("Introduzca el numero del lugar en el que se guardo el auto\nPulse 0 para seleccionar entre los ingresados\n");
		fflush(stdin);
		scanf("%d", &Auto);
		if(Auto==0)
		{
			*pEstacionados = CantidadEstacionados(pAux);//obtiene los datos de los ingresados
			printf("Introduzca el numero del lugar en el que se guardo el auto\n");
			fflush(stdin);
			scanf("%d", &Auto);
		}

		if(Auto > 0 && Auto <= 100)
		{
			for(i = 0; i<(Auto-1);i++)
				pAux++;//llevo el puntero para evaluar si en ese lugar hay un auto o no

			if(pAux->Lugar == Auto)//si hay auto
				flag = FacturarEstadia(pAux, pPesos, pDinero);

			else//si no hay auto
			{
				printf("No hay vehiculos en este lugar\nDesea volver a introducir un numero?\n");
				Confirmacion = Confirmar();
			}
		}
		else
		{
			printf("Numero incorrecto\nDesea volver a introducir un numero?\n");
			Confirmacion = Confirmar();
		}
	}while(Confirmacion == 1);

	if(flag==1)//se muestra cuando se confirma la salida del auto
	{
		(*pEstacionados)--;
		printf("La cantidad de autos estacionados es: %d\n", *pEstacionados);
		(*pEspacios)++;
		printf("La cantidad de espacios disponibles es: %d\n", *pEspacios);
	}
	else if(flag==2)//se muestra si finalmente si se introdujo mal algun dato(hora)
	{
		printf("La cantidad de autos estacionados es: %d\n", *pEstacionados);
		printf("La cantidad de espacios disponibles es: %d\n", *pEspacios);
	}
}
int FacturarEstadia(COCHE *pIngreso, PRECIO *pPesos, CAJA *pDinero)
{
	int Confirmacion, SegundosEstadia, HorasEstadia, FraccionesEstadia, PrecioParcial, PrecioFinal, Descuento, Medio;
	printf("\n***Ha seleccionado este auto***\n\n");
	printf("Lugar Estacionamiento: %d \n", pIngreso->Lugar);
	printf("Patente: %s \n", pIngreso->Patente);
	printf("Horario Entrada: %s \n", pIngreso->Entrada);
	printf("Dias Guardado: %d \n", pIngreso->DiasGuardado);
	printf("Modelo: %s \n", pIngreso->Modelo);
	printf("Desea continuar?\n");
	Confirmacion = Confirmar();
	if(Confirmacion == 1)
	{
		pIngreso->SalidaSegundos = IngresarHora(pIngreso->Salida);
		printf("Horario Salida: %s \n", pIngreso->Salida);
		SegundosEstadia = pIngreso->SalidaSegundos + (pIngreso->DiasGuardado * 86400) - pIngreso->EntradaSegundos;
		//se le suman la cantidad de dias guardado por los segundos q tiene un dia
		if(SegundosEstadia<0)
		{
			printf("El horario  de Salida no puede ser menor al de entrada\n");
			return 2;
		}
		else
		{
			HorasEstadia = SegundosEstadia / 3600;
			SegundosEstadia = SegundosEstadia - (HorasEstadia*3600);
			FraccionesEstadia = SegundosEstadia / 300;
			PrecioParcial = (HorasEstadia * pPesos->PrecioHora) + (FraccionesEstadia * pPesos->PrecioFraccion) + (pIngreso->DiasGuardado * pPesos->PrecioEstadia);
			printf("Precio Parcial %d \n", PrecioParcial);
			Medio = MedioDePago();
			printf("Posee algun descuento?\n");
			Confirmacion = Confirmar();
			if(Confirmacion == 1)
			{
					do
					{
						printf("Ingrese Descuento (00 a 99)\n");
						fflush(stdin);
						scanf("%d", &Descuento);
					}while(!(Descuento>=0 && Descuento<=99));
				PrecioFinal = PrecioParcial - ((PrecioParcial*Descuento)/100);
			}
			else
				PrecioFinal = PrecioParcial;
			printf("El precio total es: %d\n", PrecioFinal);
			if(Medio==1)
				pDinero->Efectivo = pDinero->Efectivo + PrecioFinal;
			else if(Medio==2)
				pDinero->Debito = pDinero->Debito + PrecioFinal;
			else if(Medio==3)
				pDinero->Credito = pDinero->Credito + PrecioFinal;
			else
				printf("Error. Contacte a Sistemas\n");//por precacución
			printf("\nPulse una tecla para continuar\n");
			fflush(stdin);
			getch();
			pIngreso->Lugar = 0;
			return 1;
		}
	}
	else
		return 0;
}
void PrecioPorDia(PRECIO *pPesos)
{
	system("cls");
	printf("Precio Por Estadia: %d \n", pPesos->PrecioEstadia);
	printf("Precio Por Hora: %d \n", pPesos->PrecioHora);
	printf("Precio Por Fraccion(5 Min): %d \n", pPesos->PrecioFraccion);
}
int CantidadEstacionados(COCHE *pIngreso)
{
	int i,j = 0, Estacionados = 0, Espacios;
	for(i=0; i<LUGARES; i++)
	{
		if(pIngreso->Lugar != 0)
		{
			printf("Lugar Estacionamiento: %d \n", pIngreso->Lugar);
			printf("Patente: %s \n", pIngreso->Patente);
			printf("Horario Entrada: %s \n", pIngreso->Entrada);
			printf("Dias Guardado: %d \n", pIngreso->DiasGuardado);
			printf("Modelo: %s \n\n", pIngreso->Modelo);
			Estacionados++;
		}
		j++;
		if(j<LUGARES)
			pIngreso++;
	}

	printf("La cantidad de autos estacionados es: %d\n", Estacionados);
	Espacios = LUGARES - Estacionados;
	printf("La cantidad de espacios disponibles es: %d\n", Espacios);

	return Estacionados;
}
int EspaciosDisponibles(COCHE *pIngreso)
{
	int i,j = 0, k = 0, Espacios = 0;
	system("cls");
	printf("***Lugares Disponibles*** \n");
	for(i=0; i<LUGARES; i++)
	{
		if(pIngreso->Lugar == 0)
		{
			printf("%d   ", (i+1));
			k++;
			if(k==10)//solo es para darle un poco de formato
			{
				printf("\n");
				k = 0;
			}
			Espacios++;
		}
		j++;
		if(j<LUGARES)//para que el puntero no se pase de los lugares
			pIngreso++;
	}

	printf("\nLa cantidad de espacios disponibles es: %d\n", Espacios);
	return Espacios;
}
void CierreDia(PRECIO *pPesos, CAJA *pDinero, int *pEstacionados)
{
	int Debe, Haber, AbonadoTotal, Confirmacion, PlusEstadia;
	system("cls");
	printf("Abonado en Efectivo: %d \n", pDinero->Efectivo);
	printf("Abonado en Debito: %d \n", pDinero->Debito);
	printf("Abonado en Credito: %d \n", pDinero->Credito);
	AbonadoTotal = pDinero->Efectivo + pDinero->Debito + pDinero->Credito;
	printf("\nIngresos Totales del Dia: %d \n", AbonadoTotal);

	Debe = pDinero->Efectivo + pDinero->CajaInicial;
	do
	{
	printf("\nIngrese cuanto dinero hay en la caja \n");
	fflush(stdin);
	scanf("%d", &Haber);
	Confirmacion = Confirmar();
	}while((!(Haber >= 0)) || Confirmacion == 0);
	printf("\nDEBE: %d \nHABER: %d\n", Debe, Haber);

	PlusEstadia = (*pEstacionados) * pPesos->PrecioEstadia;
	printf("\nAdemas hay %d autos estacionados que generaran un plus de %d pesos\n", *pEstacionados, PlusEstadia);
	EnviarMail(pDinero, pEstacionados, AbonadoTotal, Debe, Haber, PlusEstadia);
}

//FUNCIONES AUXILIARES
int Confirmar(void)
{
	char choose;
	do
	{
		printf("Confirme por favor s=si / n=no\n");
		fflush(stdin);
		scanf("%c", &choose);
		choose = tolower(choose);
		switch (choose)
		{
		case 's':
			return 1;
			break;
		case 'n':
			return 0;
			break;
		default:
			printf("Caracter incorrecto, por favor seleccione una opcion correcta\n");
			break;
		}
	} while (!(choose == 's' || choose == 'n'));
	return 0;
}
int IngresarHora(char *pHoraVisual)//devuelve la hora en segundos por return y por visual con el puntero
{
	int Hora, Minutos, Segundos;
	char HoraAux[3], MinutosAux[3];
	do
	{
		printf("Ingrese Hora (00 a 23)");
		fflush(stdin);
		scanf("%d", &Hora);
	}while(!(Hora>=0 && Hora<=23));
	do
	{
		printf("Ingrese Minutos (00 a 59)");
		fflush(stdin);
		scanf("%d", &Minutos);
	}while(!(Minutos>=0 && Minutos<=59));
	Segundos = (Hora*3600+Minutos*60);//ya lo tengo guardado en segundos

	//ahora pasamos a darle formato hh:mm
	if(Hora>=10)
		itoa(Hora, HoraAux, 10);//me guardo la hora en un string
	else//si es menor que diez lo coloco para que quede 07 en lugar de 7
	{
		itoa(Hora, HoraAux, 10);
		HoraAux[1] = HoraAux[0];
		HoraAux[0] = '0';
		HoraAux[2] = '\0';
	}
	if(Minutos>=10)//si es mayor a 10 solo necesito copiarlo
		itoa(Minutos, MinutosAux, 10);
	else//si es menor que diez lo coloco para que quede 07 en lugar de 7
	{
		itoa(Minutos, MinutosAux, 10);
		MinutosAux[1] = MinutosAux[0];
		MinutosAux[0] = '0';
		MinutosAux[2] = '\0';
	}
	TransformaHora(HoraAux, MinutosAux, pHoraVisual);//obtengo la cadena con el formato correcto

	return Segundos;
}
void TransformaHora(char HoraAux[3], char MinutosAux[3], char *pHoraVisual)
{
	char HoraVisual[6];
	int i;
	HoraVisual[0] = HoraAux[0];
	HoraVisual[1] = HoraAux[1];
	HoraVisual[2] = ':';
	HoraVisual[3] = MinutosAux[0];
	HoraVisual[4] = MinutosAux[1];
	HoraVisual[5] = '\0';
	for(i=0;i<6;i++)
	{
		*pHoraVisual = HoraVisual[i];
		pHoraVisual++;
	}
		
}
void EditarPrecio(PRECIO *pPesos)
{
	int i;
	printf("Precio Por Estadia:\n");
	fflush(stdin);
	scanf("%d", &i);
	pPesos->PrecioEstadia = i;

	printf("Precio Por Hora:\n");
	fflush(stdin);
	scanf("%d", &i);
	pPesos->PrecioHora = i;

	printf("Precio Por Fraccion(5 Min):\n");
	fflush(stdin);
	scanf("%d", &i);
	pPesos->PrecioFraccion = i;

	printf("Pulse una tecla para continuar\n");
	fflush(stdin);
	getch(); 
}
int MedioDePago(void)
{
	int Medio;
	do
	{
		printf("Seleccione Medio de Pago:\n1-Efectivo\n2-Debito\n3-Credito\n");
		fflush(stdin);
		scanf("%d", &Medio);
	}while(!(Medio>=1 && Medio<=3));
	return Medio;
}
void LeerDatos(COCHE *pIngreso, PRECIO *pPesos)
{
	COCHE *pIngAuxiliar;//lo uso para llamar a pIngreso al principio cuando lo necesite
	int i, j;
	pIngAuxiliar = pIngreso;
	
	FILE *Coches;
	Coches = fopen("Coches.txt", "r");
	if (Coches!= NULL)//si no existe se considera q no hay autos
	{
		do
		{
			pIngreso = pIngAuxiliar;//devuelvo al puntero al principio de la estructura Ingreso[0]
			i = 0;
			fscanf(Coches, "%d", &i);//escaneo el lugar
			if(i>0)
			{
				//ahora uso el lugar para ver en que lugar de la estructura va (ej: lugar 60 va en Ingreso[59])
				for(j=1;j<i;j++)
					pIngreso++;//coloco el puntero en el lugar de la estructura donde debe ir
				pIngreso->Lugar = i;//copio los datos donde van
				fscanf(Coches, "%s", pIngreso->Patente);
				fscanf(Coches, "%s", pIngreso->Entrada);
				fscanf(Coches, "%d", &(pIngreso->EntradaSegundos));
				fscanf(Coches, "%d", &(pIngreso->DiasGuardado));
				fscanf(Coches, "%s", pIngreso->Modelo);
			}
		}while(i>0);//solo vuelve a leer datos si i(el lugar) es mayor q cero,
		fclose(Coches);
	}
	
	FILE *Precios;
	Precios = fopen("Precios.txt", "r");
	if (Precios!= NULL)//si no existe se toman los precios en 0 para q el usuario los edite
	{
		fscanf(Precios, "%d", &(pPesos->PrecioEstadia));//escaneo precios
		fscanf(Precios, "%d", &(pPesos->PrecioHora));
		fscanf(Precios, "%d", &(pPesos->PrecioFraccion));
		fclose(Precios);
	}
}
void GuardadoDatos(COCHE *pIngreso, PRECIO *pPesos)
{
	int i;

	//Guardamos los coches en un txt
	FILE *Coches;
	Coches = fopen("Coches.txt", "w");
	for(i=0;i<LUGARES;i++)
	{
		if(pIngreso->Lugar!=0)
		{
			fprintf(Coches, "%d\n", pIngreso->Lugar);
			fprintf(Coches, "%s\n", pIngreso->Patente);
			fprintf(Coches, "%s\n", pIngreso->Entrada);
			fprintf(Coches, "%d\n", pIngreso->EntradaSegundos);
			fprintf(Coches, "%d\n", (pIngreso->DiasGuardado + 1));//suma 1 a los dias guardado del auto
			Formato(pIngreso->Modelo, 25);//para dar el formato deseado al guardar en los archivos
			fprintf(Coches, "%s\n", pIngreso->Modelo);
		}
		pIngreso++;
	}
	//Guardamos los precios en un txt
	FILE *Precios;
	Precios = fopen("Precios.txt", "w");
	fprintf(Precios, "%d\n", pPesos->PrecioEstadia);
	fprintf(Precios, "%d\n", pPesos->PrecioHora);
	fprintf(Precios, "%d\n", pPesos->PrecioFraccion);
	fclose(Precios);
}
void Formato(char *pCadena, int Tamanio)
{
	int i = 0, j = 0;
	char AuxEnt[25], AuxSal[25], *pAux;//creamos dos cadenas para copiar la cadena original y darle formato
	pAux = pCadena;

	do//COPIAMOS LA ORIGINAL EN UN AUXILIAR
	{
		AuxEnt[i] = *pCadena;
		pCadena++;
		i++;
	}while((i<Tamanio) && (AuxEnt[i-1]!='\0'));

	pCadena = pAux;
	i = 0;
	for(i=0;i<Tamanio;i++)//FORMATEAMOS LA CADENA SACANDO LOS ESPACIOS Y SIMBOLOS
	{
		if((AuxEnt[i]>='0' && AuxEnt[i]<='9') || (AuxEnt[i]>='A' && AuxEnt[i]<='Z') || (AuxEnt[i]>='a' && AuxEnt[i]<='z'))//es letra o es numero
		{
			AuxSal[j] = AuxEnt[i];
			j++;
		}
	}
	AuxSal[j] = '\0';

	i = 0;
	do//COPIAMOS LA FORMATEADA EN LA ESTRUCTURA
	{
		*pCadena = AuxSal[i];
		pCadena++;
		i++;
	}while((i<Tamanio) && (AuxSal[i-1]!='\0'));

	pCadena = pAux;
	if(*pCadena == '\0')
	{
		*pCadena = 'V';
		pCadena++;
		*pCadena = 'a';
		pCadena++;
		*pCadena = 'c';
		pCadena++;
		*pCadena = 'i';
		pCadena++;
		*pCadena = 'o';
		pCadena++;
		*pCadena = '\0';
		pCadena++;
		pCadena = pAux;
	}
}
void EnviarMail(CAJA *pDinero, int *pEstacionados, int AbonadoTotal, int Debe, int Haber, int PlusEstadia)
{
	printf("Pulse una tecla para enviar mail con toda la informacion\n");
	fflush(stdin);
	getch();
	//ServicioExterno.EnviarEmail("Estacionamiento. Cierre del dia.", "Abonado en Efectivo: %d \nAbonado en Debito: %d \nAbonado en Credito: %d \nIngresos Totales del Dia: %d \nDeberian haber $%d en la caja\nHay $%d en la caja\nAdemas hay %d autos estacionados que generaran un plus de %d pesos\n", "EstacionamientoLagash@gmail.com")
	system("cls");
	printf("Se ha enviado este mail...\nDESTINATARIO:EstacionamientoLagash@gmail.com\nASUNTO:Estacionamiento. Cierre del dia.\n\n");
	printf("Abonado en Efectivo: %d \n", pDinero->Efectivo);
	printf("Abonado en Debito: %d \n", pDinero->Debito);
	printf("Abonado en Credito: %d \n", pDinero->Credito);
	printf("Ingresos Totales del Dia: %d \n", AbonadoTotal);
	printf("Deberian haber $%d en la caja\n", Debe);
	printf("Hay $%d en la caja\n", Haber);
	printf("Ademas hay %d autos estacionados que generaran un plus de %d pesos\n", *pEstacionados, PlusEstadia);
}