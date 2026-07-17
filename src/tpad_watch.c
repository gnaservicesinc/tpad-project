/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_tree.c , is part of tpad.
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
#include <sched.h>

#define TPAD_SHA512_HEX_LENGTH 128U

// Replace deprecated pthread_yield with sched_yield
#define pthread_yield() sched_yield()
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1c = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2c = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_got_request  = PTHREAD_COND_INITIALIZER;
extern int tpad_wach_thread_set;
extern unsigned char M_HASH_SLOT_ONE[];
extern unsigned char M_HASH_SLOT_TWO[];
extern gboolean disable_file_watch;
extern int iexit;
//extern  pthread_t thread1, thread2;
extern  pthread_t rCurrent;
extern int modBalance;
extern int itog;
extern int iiit;
extern GtkSourceBuffer *mBuff;
extern gboolean save_locked;
extern GtkWidget *window;
extern gchar* origfile;
extern int madetemp;

int itog=0;
int iiit=1;
pthread_t rCurrent;
int iexit=0;
unsigned char M_HASH_SLOT_ONE[TPAD_SHA512_HEX_LENGTH + 1U];
unsigned char M_HASH_SLOT_TWO[TPAD_SHA512_HEX_LENGTH + 1U];
static void tpad_watch_copy_two_to_one();
static void tpad_watch_copy_one_to_two();
static void *tpad_watch_sig(void *arg);
static unsigned char* tpad_watch_file_hash_get(char file[]);
static void *tpad_watch_threadproc(void *arg);
static void tpad_watch_file_to_one(char* fpath);
static void tpad_watch_free_two_and_one();
static void tpad_watch_file_to_two(char* fpath);
static void tpad_watch_file_to_one(char* fpath);
static void tpad_watch_free_one();
static void tpad_watch_free_two();
static void copy_to_one(void* sString);
static void copy_to_two(void* sString);
static void tpad_watch_copy_thid(pthread_t ThisThread);
static gboolean tpad_watch_hash_is_valid(const unsigned char *hash);


static void tpad_watch_copy_thid(pthread_t ThisThread){
		pthread_mutex_lock( &mutex1c );
	 while ( pthread_mutex_trylock(&mutex2c) ){
		pthread_mutex_unlock(&mutex1c);
		pthread_yield();
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutex1c);
	}
	memcpy((void*) &rCurrent, (void*) &ThisThread,sizeof(pthread_t) );
	pthread_mutex_unlock(&mutex1c);
    	pthread_mutex_unlock(&mutex2c);

}
static void copy_to_one(void* sString){
		pthread_mutex_lock( &mutex1 );
	 while ( pthread_mutex_trylock(&mutex2) ){
		pthread_mutex_unlock(&mutex1);
		pthread_yield();
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutex1);
	}
	memcpy( (void*) M_HASH_SLOT_ONE, (void*) sString,
	        sizeof(M_HASH_SLOT_ONE));
	pthread_mutex_unlock(&mutex1);
    	pthread_mutex_unlock(&mutex2);
}
static void copy_to_two(void* sString){
		pthread_mutex_lock( &mutex1b );
	 while ( pthread_mutex_trylock(&mutex2b) ){
		pthread_mutex_unlock(&mutex1b);
		pthread_yield();
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutex1b);
	}
	memcpy( (void*) M_HASH_SLOT_TWO, (void*) sString,
	        sizeof(M_HASH_SLOT_TWO));
	pthread_mutex_unlock(&mutex1b);
    	pthread_mutex_unlock(&mutex2b);
}
static void tpad_watch_file_to_one(char* fpath){
	unsigned char *hash;

	if (fpath == NULL || strlen(fpath) <= 2)
		return;
	hash = tpad_watch_file_hash_get(fpath);
	if (tpad_watch_hash_is_valid(hash))
		copy_to_one(hash);
	else
		tpad_watch_free_one();
	g_free(hash);
}
static void tpad_watch_file_to_two(char* fpath){
	unsigned char *hash;

	if (fpath == NULL || strlen(fpath) <= 2)
		return;
	hash = tpad_watch_file_hash_get(fpath);
	if (tpad_watch_hash_is_valid(hash))
		copy_to_two(hash);
	else
		tpad_watch_free_two();
	g_free(hash);
}
static void tpad_watch_free_one(void){
	pthread_mutex_lock( &mutex1 );
	 while ( pthread_mutex_trylock(&mutex2) ){
		pthread_mutex_unlock(&mutex1);
		pthread_yield();
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutex1);
	}
	memset( (void*) M_HASH_SLOT_ONE, 0, sizeof(M_HASH_SLOT_ONE));
	pthread_mutex_unlock(&mutex1);
    	pthread_mutex_unlock(&mutex2);
}
static void tpad_watch_free_two(void){
	pthread_mutex_lock( &mutex1b );
	 while ( pthread_mutex_trylock(&mutex2b) ){
		pthread_mutex_unlock(&mutex1b);
		pthread_yield();
		sleep(WATCH_STALL_TIME);
		pthread_mutex_lock(&mutex1b);
	}
	memset( (void*) M_HASH_SLOT_TWO, 0, sizeof(M_HASH_SLOT_TWO));
	pthread_mutex_unlock(&mutex1b);
    	pthread_mutex_unlock(&mutex2b);
}

static void tpad_watch_copy_two_to_one(void){
	copy_to_one(M_HASH_SLOT_TWO);
}
static void tpad_watch_copy_one_to_two(void){
	copy_to_two(M_HASH_SLOT_ONE);
}
static void tpad_watch_free_two_and_one(void){
	tpad_watch_free_one();
	tpad_watch_free_two();
}
static void tpad_watch_check_file_init(char* fpath){
	tpad_watch_free_two_and_one();
	tpad_watch_file_to_one( (char*) fpath);
	tpad_watch_copy_one_to_two();

}
static unsigned char* tpad_watch_file_hash_get(char file[]){
	return (unsigned char *) tpad_hash_file_sha512(file);

}

static gboolean tpad_watch_hash_is_valid(const unsigned char *hash){
	return hash != NULL &&
	       strlen((const char *) hash) == TPAD_SHA512_HEX_LENGTH;
}

static  void *tpad_watch_threadproc(void *arg){
	tpad_watch_copy_thid((pthread_t) pthread_self() );
	if (disable_file_watch) return(NULL);

	if(iexit){
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	}

	char* fpath = (char*) arg;
	tpad_watch_check_file_init(fpath);
if(iexit){
		tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
}

do{
	if(iexit){
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	}
	pthread_mutex_lock( &mutex3 );
		if(iexit){
		tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
		pthread_mutex_unlock(&mutex3);
		 return(NULL);
		}
	 while ( pthread_mutex_trylock(&mutex4) ){
		pthread_mutex_unlock(&mutex3);

		if(iexit) {
			tpad_modbal_set_zero();
			tpad_watch_free_two_and_one();
			return(NULL);
			}
		else pthread_yield();

		struct timespec asleeprqtp;
		asleeprqtp.tv_sec  = 0;
		asleeprqtp.tv_nsec = 500000000L;
		if(nanosleep(&asleeprqtp , NULL) != 0 ) sleep(WATCH_PROC_SLEEP_LENGTH);
		if(iexit) {
			tpad_modbal_set_zero();
			tpad_watch_free_two_and_one();
			return(NULL);
			}
		else pthread_yield();

		pthread_mutex_lock(&mutex3);
	}

		if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}


	if(!tpad_modbal_check_armed()){
	unsigned char *current_hash = tpad_watch_file_hash_get(fpath);
	gboolean hash_changed = !tpad_watch_hash_is_valid(current_hash) ||
	                        memcmp(M_HASH_SLOT_ONE, current_hash,
	                               sizeof(M_HASH_SLOT_ONE)) != 0;
	g_free(current_hash);

	if (hash_changed){
		fprintf(stdout,"FILE CHANGED!\n");
		fflush(stdout);
		if(iexit) {
		tpad_modbal_set_zero();
		tpad_watch_free_two_and_one();
		pthread_mutex_unlock(&mutex3);
    		pthread_mutex_unlock(&mutex4);
		return(NULL);
		}
		else tpad_modbal_set_one();

		tpad_watch_check_file_init(fpath);
	  }else pthread_yield();
	}else pthread_yield();
		if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}
	//else {
	//IS Armed

	//}
	struct timespec lsleeprqtp;
			if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}
	lsleeprqtp.tv_sec  = 0;
			if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}
	lsleeprqtp.tv_nsec = 500000000L;
			if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}
	if(nanosleep(&lsleeprqtp , NULL) != 0 ) sleep(WATCH_PROC_SLEEP_LENGTH);
			if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	return(NULL);
	}
	pthread_mutex_unlock(&mutex3);
    	pthread_mutex_unlock(&mutex4);
	if(iexit) {
	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
	return(NULL);
	} else pthread_yield();
   } while(!iexit);

	tpad_modbal_set_zero();
	tpad_watch_free_two_and_one();
return(NULL);

}

void tpad_watch_exit(void){

// Disabled for now //
/*
	iexit=1;
	if(rCurrent && tpad_wach_thread_set) pthread_join((pthread_t)rCurrent,NULL);
	tpad_wach_thread_set=FALSE;
	rCurrent=(pthread_t) NULL;
*/
}

void tpad_watch_fname(void){
// Disabled for now //
/*
	pthread_t thread;
	tpad_wach_thread_set=TRUE;
	pthread_create( &thread, NULL, &tpad_watch_threadproc, (void*) tpad_fp_get_current());
	*/

}
