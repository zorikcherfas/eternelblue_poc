//
//  smbHeader.hpp
//  eternalblue_poc
//
//  Created by Zorik Cherfas on 06/08/2017.
//  Copyright © 2017 Zorik Cherfas. All rights reserved.
//

#ifndef smbHeader_hpp
#define smbHeader_hpp

#include <stdio.h>
#include<sys/socket.h>
#include<unistd.h>    //write
#include <cstring>

#define UPLOAD_BUFSIZE 1400
#define BUFSIZE 400
#define MAX_MESSAGE_SIZE 400
enum smb_conn_state {
    SMB_NOT_CONNECTED = 0,
    SMB_CONNECTING,
    SMB_NEGOTIATE,
    SMB_SETUP,
    SMB_CONNECTED
};

struct smb_conn {
    enum smb_conn_state state;
    char *user;
    char *domain;
    unsigned char challenge[8];
    unsigned int session_key;
    unsigned short uid;
    char *recv_buf;
    size_t upload_size;
    size_t send_size;
    size_t sent;
    size_t got;
};




class SMB{
public:
    int m_socket;
    struct smb_conn m_connection;
    char m_uploadbuffer[UPLOAD_BUFSIZE+1];
    char m_downloadbuffer[UPLOAD_BUFSIZE+1];

    SMB(int socket){
        m_socket = socket;
        memset(this->m_downloadbuffer, 0, sizeof(this->m_downloadbuffer));
        memset(this->m_uploadbuffer, 0, sizeof(this->m_uploadbuffer));
        memset(&this->m_connection, 0, sizeof(this->m_connection));
        
        this->m_connection.user = strdup("ZORIK-PC");
        this->m_connection.domain = strdup("WORKGROUP");
        this->initDefaultConnectionState();

    }
    ~SMB(){
        
        if(this->m_connection.user)
            delete(this->m_connection.user);
        
        if(this->m_connection.domain)
            delete(this->m_connection.domain);
        
        if(m_socket)
            close(m_socket);
    }
    
    void initDefaultConnectionState(){
        this->m_connection.state = smb_conn_state::SMB_NOT_CONNECTED;
    }
    
    void setConnectionState(smb_conn_state newState){
        this->m_connection.state = newState;
    }
    smb_conn_state getConnectionState(){
        return this->m_connection.state;
    }
    
    int smb_send_negotiate();
    int smb_send_and_recv();
    int smb_send_setup();

    
private:
    int smb_send_message(unsigned char cmd,
                         const void *msg, size_t msg_len);
    int smb_send(size_t len,
                 size_t upload_size);
    void smb_format_message( struct smb_header *h,
                            unsigned char cmd, size_t len);
    int smb_format_setup(struct smb_setup *msg);
    int smb_recv_message(void **msg);
    
    void printWorkGroup(struct smb_negotiate_response *h);
};




struct __attribute__((__packed__)) smb_header {
    unsigned char nbt_type;
    unsigned char nbt_flags;
    unsigned short nbt_length;
    unsigned char magic[4];
    unsigned char command;
    unsigned int status;
    unsigned char flags;
    unsigned short flags2;
    unsigned short pid_high;
    unsigned char signature[8];
    unsigned short pad;
    unsigned short tid;
    unsigned short pid;
    unsigned short uid;
    unsigned short mid;
} ;

struct __attribute__ ((__packed__)) smb_negotiate_response  {
    struct smb_header h;
    unsigned char word_count;
    unsigned short dialect_index;
    unsigned char security_mode;
    unsigned short max_mpx_count;
    unsigned short max_number_vcs;
    unsigned int max_buffer_size;
    unsigned int max_raw_size;
    unsigned int session_key;
    unsigned int capabilities;
    unsigned int system_time_low;
    unsigned int system_time_high;
    unsigned short server_time_zone;
    unsigned char encryption_key_length;
    unsigned short byte_count;
    char bytes[1];
};


struct __attribute__ ((__packed__)) andx {
    unsigned char command;
    unsigned char pad;
    unsigned short offset;
} ;

struct __attribute__ ((__packed__)) smb_setup {
    unsigned char word_count;
    struct andx andx;
    unsigned short max_buffer_size;
    unsigned short max_mpx_count;
    unsigned short vc_number;
    unsigned int session_key;
    unsigned short lengths[2];
    unsigned int pad;
    unsigned int capabilities;
    unsigned short byte_count;
    char bytes[1200];
} ;

#define SMB_COM_CLOSE                 0x04
#define SMB_COM_READ_ANDX             0x2e
#define SMB_COM_WRITE_ANDX            0x2f
#define SMB_COM_TREE_DISCONNECT       0x71
#define SMB_COM_NEGOTIATE             0x72
#define SMB_COM_SETUP_ANDX            0x73
#define SMB_COM_TREE_CONNECT_ANDX     0x75
#define SMB_COM_NT_CREATE_ANDX        0xa2
#define SMB_COM_NO_ANDX_COMMAND       0xff

#define SMB_WC_CLOSE                  0x03
#define SMB_WC_READ_ANDX              0x0c
#define SMB_WC_WRITE_ANDX             0x0e
#define SMB_WC_SETUP_ANDX             0x0d
#define SMB_WC_TREE_CONNECT_ANDX      0x04
#define SMB_WC_NT_CREATE_ANDX         0x18

#define SMB_FLAGS_CANONICAL_PATHNAMES 0x10
#define SMB_FLAGS_CASELESS_PATHNAMES  0x08
#define SMB_FLAGS2_UNICODE_STRINGS    0x8000
#define SMB_FLAGS2_IS_LONG_NAME       0x0040
#define SMB_FLAGS2_KNOWS_LONG_NAME    0x0001

#define SMB_CAP_LARGE_FILES           0x08
#define SMB_GENERIC_WRITE             0x40000000
#define SMB_GENERIC_READ              0x80000000
#define SMB_FILE_SHARE_ALL            0x07
#define SMB_FILE_OPEN                 0x01
#define SMB_FILE_OVERWRITE_IF         0x05

#define SMB_ERR_NOACCESS              0x00050001

/*
 * Definitions for SMB protocol data structures
 */
#ifdef BUILDING_CURL_SMB_C
#error
#if defined(_MSC_VER) || defined(__ILEC400__)
#  define PACK
#  pragma pack(push)
#  pragma pack(1)
#elif defined(__GNUC__)
#  define PACK __attribute__((packed))
#else
#  define PACK
#endif









struct smb_tree_connect {
    unsigned char word_count;
    struct andx andx;
    unsigned short flags;
    unsigned short pw_len;
    unsigned short byte_count;
    char bytes[1024];
} PACK;

struct smb_nt_create {
    unsigned char word_count;
    struct andx andx;
    unsigned char pad;
    unsigned short name_length;
    unsigned int flags;
    unsigned int root_fid;
    unsigned int access;
#ifdef HAVE_LONGLONG
    unsigned long long allocation_size;
#else
    unsigned __int64 allocation_size;
#endif
    unsigned int ext_file_attributes;
    unsigned int share_access;
    unsigned int create_disposition;
    unsigned int create_options;
    unsigned int impersonation_level;
    unsigned char security_flags;
    unsigned short byte_count;
    char bytes[1024];
} PACK;

struct smb_nt_create_response {
    struct smb_header h;
    unsigned char word_count;
    struct andx andx;
    unsigned char op_lock_level;
    unsigned short fid;
    unsigned int create_disposition;
#ifdef HAVE_LONGLONG
    unsigned long long create_time;
    unsigned long long last_access_time;
    unsigned long long last_write_time;
    unsigned long long last_change_time;
#else
    unsigned __int64 create_time;
    unsigned __int64 last_access_time;
    unsigned __int64 last_write_time;
    unsigned __int64 last_change_time;
#endif
    unsigned int ext_file_attributes;
#ifdef HAVE_LONGLONG
    unsigned long long allocation_size;
    unsigned long long end_of_file;
#else
    unsigned __int64 allocation_size;
    unsigned __int64 end_of_file;
#endif
} PACK;

struct smb_read {
    unsigned char word_count;
    struct andx andx;
    unsigned short fid;
    unsigned int offset;
    unsigned short max_bytes;
    unsigned short min_bytes;
    unsigned int timeout;
    unsigned short remaining;
    unsigned int offset_high;
    unsigned short byte_count;
} PACK;

struct smb_write {
    struct smb_header h;
    unsigned char word_count;
    struct andx andx;
    unsigned short fid;
    unsigned int offset;
    unsigned int timeout;
    unsigned short write_mode;
    unsigned short remaining;
    unsigned short pad;
    unsigned short data_length;
    unsigned short data_offset;
    unsigned int offset_high;
    unsigned short byte_count;
    unsigned char pad2;
} PACK;

struct smb_close {
    unsigned char word_count;
    unsigned short fid;
    unsigned int last_mtime;
    unsigned short byte_count;
} PACK;

struct smb_tree_disconnect {
    unsigned char word_count;
    unsigned short byte_count;
} PACK;

#if defined(_MSC_VER) || defined(__ILEC400__)
#  pragma pack(pop)
#endif

#endif /* BUILDING_CURL_SMB_C */

#if !defined(CURL_DISABLE_SMB) && defined(USE_NTLM) && \
(CURL_SIZEOF_CURL_OFF_T > 4)

#if !defined(USE_WINDOWS_SSPI) || defined(USE_WIN32_CRYPTO)

extern const struct Curl_handler Curl_handler_smb;
extern const struct Curl_handler Curl_handler_smbs;

#endif /* !USE_WINDOWS_SSPI || USE_WIN32_CRYPTO */

#endif /* CURL_DISABLE_SMB && USE_NTLM && CURL_SIZEOF_CURL_OFF_T > 4 */


#endif /* HEADER_CURL_SMB_H */

