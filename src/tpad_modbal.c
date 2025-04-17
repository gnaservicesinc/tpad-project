/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_modbal.c , is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   tpad is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with tpad.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************/
#include "tpad_headers.h"
#include <pthread.h>

extern pthread_mutex_t mutexA;
extern pthread_mutex_t mutexB;
pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;
extern int mBalance;
int mBalance=0;

static int modbal_armed ();
static void modbal_one();
static void modbal_neg();
static void modbal_zero();
static int modbal_get();  
/*
void tpad_modbal_set_zero(){ modbal_zero();}
void tpad_modbal_set_one(){ modbal_one();}
void tpad_modbal_set_neg(){ modbal_neg();}
int tpad_modbal_check_armed(){ return(modbal_armed());}
int tpad_modbal_get(){ return(modbal_get());}
*/
void tpad_modbal_set_zero(){ int a=0;}
void tpad_modbal_set_one(){ int a=0;}
void tpad_modbal_set_neg(){ int a=0;}
int tpad_modbal_check_armed(){ return(0);}
int tpad_modbal_get(){ return(0);}

static int modbal_armed () {
	int iRes=0;
	  pthread_mutex_lock( &mutexA );
	 while ( pthread_mutex_trylock(&mutexB) ){
		pthread_mutex_unlock(&mutexA); 
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutexA);
	}
	if(mBalance > 0) iRes = 1;
	else iRes=0;

	//fprintf(stdout,"\nmBalance (modbal_armed ()) = %i\n",mBalance);
	pthread_mutex_unlock(&mutexA);
    	pthread_mutex_unlock(&mutexB);
	return(iRes);
}
static void modbal_zero() {

	  pthread_mutex_lock( &mutexA );
	 while ( pthread_mutex_trylock(&mutexB) ){
		pthread_mutex_unlock(&mutexA); 
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutexA);
	}
	mBalance=0;
	//fprintf(stdout,"\nmBalance = %i\n",mBalance);
	pthread_mutex_unlock(&mutexA);
    	pthread_mutex_unlock(&mutexB);
}

static void modbal_one() {
	pthread_mutex_lock( &mutexA );
	 while ( pthread_mutex_trylock(&mutexB) ){
		pthread_mutex_unlock(&mutexA); 
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutexA);
	}
	mBalance++;
	//fprintf(stdout,"\nmBalance = %i\n",mBalance);
	pthread_mutex_unlock(&mutexA);
    	pthread_mutex_unlock(&mutexB);
}

static void modbal_neg() {
		  pthread_mutex_lock( &mutexA );
	 while ( pthread_mutex_trylock(&mutexB) ){
		pthread_mutex_unlock(&mutexA); 
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutexA);
	}
	 mBalance--;
	//fprintf(stdout,"\nmBalance = %i\n",mBalance);
	pthread_mutex_unlock(&mutexA);
    	pthread_mutex_unlock(&mutexB);
}

static int modbal_get() {
	int iRets=0;
	 pthread_mutex_lock( &mutexA );
	 while ( pthread_mutex_trylock(&mutexB) ){
		pthread_mutex_unlock(&mutexA); 
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutexA);
	}
	iRets=mBalance;
	//fprintf(stdout,"\nmBalance (mBalance get req) = %i\n",mBalance);
	pthread_mutex_unlock(&mutexA);
    	pthread_mutex_unlock(&mutexB);

	return(iRets);
}
