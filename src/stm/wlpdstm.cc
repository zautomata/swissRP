//Copyright (c) 2010, Aleksandar Dragojevic aleksandar.dragojevic@epfl.ch
//All rights reserved.
//
//Changes to support Relativistic Programming Copyright (c) Philip W. Howard
//phil.w.howard@gmail.com
//All rights reserved.
//
//This software is release under the terms of the BSD License as follows:
//
//Redistribution and use in source and binary forms, with or without 
//modification, are permitted provided that the following conditions are 
//met:
//
//Redistributions of source code must retain the above copyright notice, 
//this list of conditions and the following disclaimer.
//
//Redistributions in binary form must reproduce the above copyright notice, 
//this list of conditions and the following disclaimer in the documentation 
//and/or other materials provided with the distribution.
//
//Neither the name of the <ORGANIZATION> nor the names of its contributors 
//may be used to endorse or promote products derived from this software 
//without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
//ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
//LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//POSSIBILITY OF SUCH DAMAGE.
//
/** 
 * @author Aleksandar Dragojevic aleksandar.dragojevic@epfl.ch
 */

#include "wlpdstm.h"

#include "tid.h"
#include "memory.h"
#include "transaction.h"
#include "constants.h"
#include "read_write.h"
#include "wlpdstm_malloc.h"

///////////////////////////
// basic interface start //
///////////////////////////	

extern "C" {

char *wlpdstm_version() {
#ifdef RP_STM
    return "swissRP";
#else
    return "swissTM";
#endif
}

void wlpdstm_global_init() {
	wlpdstm::CurrentTransaction::GlobalInit();
}

void wlpdstm_thread_init() {
	wlpdstm::CurrentTransaction::ThreadInit();
}

void wlpdstm_start_tx() {
	wlpdstm::CurrentTransaction::TxStart();
}

void wlpdstm_start_tx_id(int lexical_tx_id) {
	wlpdstm::CurrentTransaction::TxStart(lexical_tx_id);
}

LONG_JMP_BUF *wlpdstm_get_long_jmp_buf() {
	return wlpdstm::CurrentTransaction::GetLongJmpBuf();
}

void wlpdstm_commit_tx() {
	wlpdstm::CurrentTransaction::TxCommit();
}

void wlpdstm_abort_tx() {
	wlpdstm::CurrentTransaction::TxAbort();
}

void wlpdstm_restart_tx() {
	wlpdstm::CurrentTransaction::TxRestart();
}

void wlpdstm_write_word(Word *address, Word value) {
	wlpdstm::CurrentTransaction::WriteWord(address, value);
}

#ifdef RP_STM
void wlpdstm_write_word_mb(Word *address, Word value) {
	wlpdstm::CurrentTransaction::WriteWord_mb(address, value);
}

void wlpdstm_free(void *address) {
	wlpdstm::CurrentTransaction::Free(address);
}

void wlpdstm_rp_free(void *rp_context, void *address) {
	wlpdstm::CurrentTransaction::RPFree(rp_context, address);
}

void wlpdstm_grace_period(void *rp_context) {
    wlpdstm::CurrentTransaction::GracePeriod(rp_context);
}
#endif

#ifdef SUPPORT_LOCAL_WRITES
void wlpdstm_write_word_local(Word *address, Word value) {
	wlpdstm::CurrentTransaction::WriteWordLocal(address, value);
}
#endif /* SUPPORT_LOCAL_WRITES */

Word wlpdstm_read_word(Word *address) {
	return wlpdstm::CurrentTransaction::ReadWord(address);
}

void wlpdstm_tx_free(void *ptr, size_t size) {
	wlpdstm_malloc_tx_free(ptr, size);
}

void *wlpdstm_tx_malloc(size_t size) {
	return wlpdstm_malloc_tx_malloc(size);
}

tx_desc *wlpdstm_get_tx_desc() {
	return (::tx_desc *)wlpdstm::CurrentTransaction::Get();
}

void wlpdstm_start_tx_desc(tx_desc *tx) {
	((wlpdstm::Transaction *)tx)->TxStart();
}

LONG_JMP_BUF *wlpdstm_get_long_jmp_buf_desc(tx_desc *tx) {
	return &((wlpdstm::Transaction *)tx)->start_buf;
}

void wlpdstm_start_tx_id_desc(tx_desc *tx, int lexical_tx_id) {
	((wlpdstm::Transaction *)tx)->TxStart(lexical_tx_id);
}

void wlpdstm_commit_tx_desc(tx_desc *tx) {
	((wlpdstm::Transaction *)tx)->TxCommit();
}

void wlpdstm_restart_tx_desc(tx_desc *tx) {
	((wlpdstm::Transaction *)tx)->TxRestart();
}

void wlpdstm_abort_tx_desc(tx_desc *tx) {
	((wlpdstm::Transaction *)tx)->TxAbort();
}

Word wlpdstm_read_word_desc(tx_desc *tx, Word *address) {
	return ((wlpdstm::Transaction *)tx)->ReadWord(address);
}

void wlpdstm_write_word_desc(tx_desc *tx, Word *address, Word value) {
	((wlpdstm::Transaction *)tx)->WriteWord(address, value);
}

#ifdef SUPPORT_LOCAL_WRITES
void wlpdstm_write_word_local_desc(tx_desc *tx, Word *address, Word value) {
	((wlpdstm::Transaction *)tx)->WriteWordLocal(address, value);
}
#endif /* SUPPORT_LOCAL_WRITES */

void wlpdstm_tx_free_desc(tx_desc *tx, void *ptr, size_t size) {
	((wlpdstm::Transaction *)tx)->TxFree(ptr, size);
}

void *wlpdstm_tx_malloc_desc(tx_desc *tx, size_t size) {
	return ((wlpdstm::Transaction *)tx)->TxMalloc(size);
}

void wlpdstm_print_stats() {
	wlpdstm::Transaction::PrintStatistics();
}

void *wlpdstm_s_malloc(size_t size) {
	return wlpdstm::MemoryManager::Malloc(size);
}

void wlpdstm_s_free(void *ptr) {
	wlpdstm::MemoryManager::Free(ptr);
}

/////////////////////////
// basic interface end //
/////////////////////////	


//////////////////////////////
// extended interface start //
//////////////////////////////

uint32_t wlpdstm_read_32(uint32_t *address) {
	return read32aligned(wlpdstm::CurrentTransaction::Get(), address);
}

uint32_t wlpdstm_read_32_desc(tx_desc *tx, uint32_t *address) {
	return read32aligned((wlpdstm::Transaction *)tx, address);
}

float wlpdstm_read_float(float *address) {
	return read_float_aligned(wlpdstm::CurrentTransaction::Get(), address);
}

float wlpdstm_read_float_desc(tx_desc *tx, float *address) {
	return read_float_aligned((wlpdstm::Transaction *)tx, address);
}

uint64_t wlpdstm_read_64(uint64_t *address) {
	return read64aligned(wlpdstm::CurrentTransaction::Get(), address);
}

uint64_t wlpdstm_read_64_desc(tx_desc *tx, uint64_t *address) {
	return read64aligned((wlpdstm::Transaction *)tx, address);
}

double wlpdstm_read_double(double *address) {
	return read_double_aligned(wlpdstm::CurrentTransaction::Get(), address);
}

double wlpdstm_read_double_desc(tx_desc *tx, double *address) {
	return read_double_aligned((wlpdstm::Transaction *)tx, address);
}

void wlpdstm_write_32(uint32_t *address, uint32_t value) {
	write32aligned(wlpdstm::CurrentTransaction::Get(), address, value);
}

void wlpdstm_write_32_desc(tx_desc *tx, uint32_t *address, uint32_t value) {
	write32aligned((wlpdstm::Transaction *)tx, address, value);
}

void wlpdstm_write_float(float *address, float value) {
	write_float_aligned(wlpdstm::CurrentTransaction::Get(), address, value);
}

void wlpdstm_write_float_desc(tx_desc *tx, float *address, float value) {
	write_float_aligned((wlpdstm::Transaction *)tx, address, value);
}

void wlpdstm_write_64(uint64_t *address, uint64_t value) {
	write64aligned(wlpdstm::CurrentTransaction::Get(), address, value);
}

void wlpdstm_write_64_desc(tx_desc *tx, uint64_t *address, uint64_t value) {
	write64aligned((wlpdstm::Transaction *)tx, address, value);
}

void wlpdstm_write_double(double *address, double value) {
	write_double_aligned(wlpdstm::CurrentTransaction::Get(), address, value);
}

void wlpdstm_write_double_desc(tx_desc *tx, double *address, double value) {
	write_double_aligned((wlpdstm::Transaction *)tx, address, value);
}

}

//////////////////////////////
// extended interface end //
//////////////////////////////
