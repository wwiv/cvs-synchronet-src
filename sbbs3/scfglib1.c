/* scfglib1.c */

/* Synchronet configuration library routines */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2000 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#include "sbbs.h"
#include "scfglib.h"

const char *scfgnulstr="";

void DLLEXPORT prep_dir(char* base, char* path)
{
#ifdef __unix__
	char	*p;
#endif
	char	str[LEN_DIR*2];

	if(!path[0])
		return;
	if(path[0]!='\\' && path[0]!='/' && path[1]!=':')           /* Relative to NODE directory */
		sprintf(str,"%s%s",base,path);
	else
		strcpy(str,path);

#ifdef __unix__				/* Change backslashes to forward slashes on Unix */
	for(p=str;*p;p++)
		if(*p=='\\') 
			*p='/';
	strlwr(str);	/* temporary hack */
#endif

	backslashcolon(str);
	strcat(str,".");                // Change C: to C:. and C:\SBBS\ to C:\SBBS\.
	_fullpath(path,str,LEN_DIR+1);	// Change C:\SBBS\NODE1\..\EXEC to C:\SBBS\EXEC
	backslash(path);
}

void prep_path(char* path)
{
#ifdef __unix__				/* Change backslashes to forward slashes on Unix */
	char	*p;

	for(p=path;*p;p++)
		if(*p=='\\') 
			*p='/';
#endif
}

char * get_alloc(long *offset, char *outstr, int maxlen, FILE *instream)
{

#ifdef SCFG
	fread(outstr,1,maxlen+1,instream);
	(*offset)+=maxlen+1;
#else
	char str[257];

	fread(str,1,maxlen+1,instream);
//	  lprintf("%s %d %p\r\n",__FILE__,__LINE__,offset);
	(*offset)+=maxlen+1;	// this line was commented out (04/12/97) why?
	if(!str[0]) 			/* Save memory */
		return((char *)scfgnulstr);
	if((outstr=(char *)MALLOC(strlen(str)+1))==NULL)
		return(NULL);
	strcpy(outstr,str);
#endif

return(outstr);
}

#ifdef SCFG 	/* SCFG allocate max length */
	#define readline_alloc(l,s,m,i) readline(l,s,m,i)
#else
	char *readline_alloc(long *offset, char *outstr, int maxline
		, FILE *instream);
	#define readline_alloc(l,s,m,i) s=readline_alloc(l,s,m,i)
	#define get_alloc(o,s,l,i) s=get_alloc(o,s,l,i)
#endif

/***********************************************************/
/* These functions are called from here and must be linked */
/***********************************************************/
/***
	nopen()
	truncsp()
***/

BOOL allocerr(read_cfg_text_t* txt, long offset, char *fname, uint size)
{
	lprintf(txt->error,offset,fname);
	lprintf(txt->allocerr,size);
	return(FALSE);
}

#ifndef NO_NODE_CFG

/****************************************************************************/
/* Reads in NODE.CNF and initializes the associated variables				*/
/****************************************************************************/
BOOL read_node_cfg(scfg_t* cfg, read_cfg_text_t* txt)
{
	char	c,str[256],fname[13];
	int 	i;
	short	n;
	long	offset=0;
	FILE	*instream;

	strcpy(fname,"node.cnf");
	sprintf(str,"%s%s",cfg->node_dir,fname);
	if((instream=fnopen(NULL,str,O_RDONLY))==NULL) {
		lprintf(txt->openerr,str);
		return(FALSE); 
	}

	if(txt->reading && txt->reading[0])
		lprintf(txt->reading,fname);
	get_int(cfg->node_num,instream);
	if(!cfg->node_num) {
		lprintf(txt->error,offset,fname);
		lprintf("Node number must be non-zero.");
		return(FALSE); }
	get_str(cfg->node_name,instream);
	get_str(cfg->node_phone,instream);
	get_str(cfg->node_comspec,instream);
	#ifdef __OS2__
	if(!node_comspec[0])
		strcpy(node_comspec,"C:\\OS2\\MDOS\\COMMAND.COM");
	#endif
	get_int(cfg->node_misc,instream);
	get_int(cfg->node_ivt,instream);
	get_int(cfg->node_swap,instream);
	get_str(cfg->node_swapdir,instream);
	get_int(cfg->node_valuser,instream);
	get_int(cfg->node_minbps,instream);
	#ifdef SCFG
	get_str(cfg->node_ar,instream);
	#else
	fread(str,1,LEN_ARSTR+1,instream);
	offset+=LEN_ARSTR+1;
	cfg->node_ar=arstr(0, str, cfg);
	#endif
	get_int(cfg->node_dollars_per_call,instream);
	get_str(cfg->node_editor,instream);
	get_str(cfg->node_viewer,instream);
	get_str(cfg->node_daily,instream);
	get_int(c,instream);
	if(c) cfg->node_scrnlen=c;
	get_int(cfg->node_scrnblank,instream);
	get_str(cfg->text_dir,instream); 				/* ctrl directory */
	get_str(cfg->text_dir,instream); 				/* text directory */
	get_str(cfg->temp_dir,instream); 				/* temp directory */
	if(!cfg->temp_dir[0])
		strcpy(cfg->temp_dir,"temp");

	strlwr(cfg->text_dir);	/* temporary Unix-compatibility hack */
	strlwr(cfg->temp_dir);	/* temporary Unix-compatibility hack */

	#ifndef SCFG
	prep_dir(cfg->ctrl_dir, cfg->text_dir);
	#endif

	for(i=0;i<10;i++) { 						/* WFC 0-9 DOS commands */
		get_alloc(&offset,cfg->wfc_cmd[i],LEN_CMD,instream); }
	for(i=0;i<12;i++) { 						/* WFC F1-F12 shrinking DOS cmds */
		get_alloc(&offset,cfg->wfc_scmd[i],LEN_CMD,instream); }
	get_str(cfg->mdm_hang,instream);
	get_int(cfg->node_sem_check,instream);
	if(!cfg->node_sem_check) cfg->node_sem_check=60;
	get_int(cfg->node_stat_check,instream);
	if(!cfg->node_stat_check) cfg->node_stat_check=10;
	get_str(cfg->scfg_cmd,instream);
	if(!cfg->scfg_cmd[0])
		strcpy(cfg->scfg_cmd,"%!scfg %k");
	get_int(cfg->sec_warn,instream);
	if(!cfg->sec_warn)
		cfg->sec_warn=180;
	get_int(cfg->sec_hangup,instream);
	if(!cfg->sec_hangup)
		cfg->sec_hangup=300;
	for(i=0;i<188;i++) {				/* Unused - initialized to NULL */
		fread(&n,1,2,instream);
		offset+=2; }
	for(i=0;i<256;i++) {				/* Unused - initialized to 0xff */
		fread(&n,1,2,instream);
		offset+=2; }

	/***************/
	/* Modem Stuff */
	/***************/

	get_int(cfg->com_port,instream);
	get_int(cfg->com_irq,instream);
	get_int(cfg->com_base,instream);
	get_int(cfg->com_rate,instream);
	get_int(cfg->mdm_misc,instream);
	get_str(cfg->mdm_init,instream);
	get_str(cfg->mdm_spec,instream);
	get_str(cfg->mdm_term,instream);
	get_str(cfg->mdm_dial,instream);
	get_str(cfg->mdm_offh,instream);
	get_str(cfg->mdm_answ,instream);
	get_int(cfg->mdm_reinit,instream);
	get_int(cfg->mdm_ansdelay,instream);
	get_int(cfg->mdm_rings,instream);

	get_int(cfg->mdm_results,instream);

	if(cfg->mdm_results) {
		if((cfg->mdm_result=(mdm_result_t *)MALLOC(sizeof(mdm_result_t)*cfg->mdm_results))
			==NULL)
		return allocerr(txt,offset,fname,sizeof(mdm_result_t *)*cfg->mdm_results); }
	else
		cfg->mdm_result=NULL;

	for(i=0;i<cfg->mdm_results;i++) {
		if(feof(instream)) break;
		get_int(cfg->mdm_result[i].code,instream);
		get_int(cfg->mdm_result[i].rate,instream);
		get_int(cfg->mdm_result[i].cps,instream);
		get_alloc(&offset,cfg->mdm_result[i].str,LEN_MODEM,instream); }
	cfg->mdm_results=i;
	fclose(instream);
	if(txt->readit && txt->readit[0])
		lprintf(txt->readit,fname);
	return(TRUE);
}

#endif

#ifndef NO_MAIN_CFG

/****************************************************************************/
/* Reads in MAIN.CNF and initializes the associated variables				*/
/****************************************************************************/
BOOL read_main_cfg(scfg_t* cfg, read_cfg_text_t* txt)
{
	char	str[256],fname[13],c;
	short	i,j,n;
	long	offset=0;
	FILE	*instream;

	strcpy(fname,"main.cnf");
	sprintf(str,"%s%s",cfg->ctrl_dir,fname);
	if((instream=fnopen(NULL,str,O_RDONLY))==NULL) {
		lprintf(txt->openerr,str);
		return(FALSE); }

	if(txt->reading && txt->reading[0])
		lprintf(txt->reading,fname);
	get_str(cfg->sys_name,instream);
	get_str(cfg->sys_id,instream);
	get_str(cfg->sys_location,instream);
	get_str(cfg->sys_phonefmt,instream);
	get_str(cfg->sys_op,instream);
	get_str(cfg->sys_guru,instream);
	get_str(cfg->sys_pass,instream);
	get_int(cfg->sys_nodes,instream);

	if(!cfg->sys_nodes || cfg->sys_nodes<cfg->node_num || cfg->sys_nodes>MAX_NODES) {
		lprintf(txt->error,offset,fname);
		if(!cfg->sys_nodes)
			lprintf("Total nodes on system must be non-zero.");
		else if(cfg->sys_nodes>MAX_NODES)
			lprintf("Total nodes exceeds %u.",MAX_NODES);
		else
			lprintf("Total nodes (%u) < node number in NODE.CNF (%u)"
				,cfg->sys_nodes,cfg->node_num);
		return(FALSE); }

	if((cfg->node_path=(char **)MALLOC(sizeof(char *)*cfg->sys_nodes))==NULL)
		return allocerr(txt,offset,fname,sizeof(char *)*cfg->sys_nodes);

	for(i=0;i<cfg->sys_nodes;i++) {
		if(feof(instream)) break;
		fread(str,LEN_DIR+1,1,instream);
		prep_dir(cfg->ctrl_dir, str);
		offset+=LEN_DIR+1;
		if((cfg->node_path[i]=(char *)MALLOC(strlen(str)+1))==NULL)
			return allocerr(txt,offset,fname,strlen(str)+1);
		strcpy(cfg->node_path[i],str); 
		strlwr(cfg->node_path[i]); /* temporary Unix-compatibility hack */
	}

	get_str(cfg->data_dir,instream); 			  /* data directory */
	get_str(cfg->exec_dir,instream); 			  /* exec directory */

	strlwr(cfg->data_dir);		/* temporary Unix-compatibility hack */
	strlwr(cfg->exec_dir);		/* temporary Unix-compatibility hack */

	#ifndef SCFG
	prep_dir(cfg->ctrl_dir, cfg->data_dir);
	prep_dir(cfg->ctrl_dir, cfg->exec_dir);
	#endif

	get_str(cfg->sys_logon,instream);
	get_str(cfg->sys_logout,instream);
	get_str(cfg->sys_daily,instream);
	get_int(cfg->sys_timezone,instream);
	get_int(cfg->sys_misc,instream);
	get_int(cfg->sys_lastnode,instream);
	get_int(cfg->sys_autonode,instream);
	get_int(cfg->uq,instream);
	get_int(cfg->sys_pwdays,instream);
	get_int(cfg->sys_deldays,instream);
	get_int(cfg->sys_exp_warn,instream); 	/* Days left till expiration warning */
	get_int(cfg->sys_autodel,instream);
	get_int(cfg->sys_def_stat,instream); 	/* default status line */

	#ifdef SCFG
	get_str(cfg->sys_chat_ar,instream);
	#else
	fread(str,1,LEN_ARSTR+1,instream);
	offset+=LEN_ARSTR+1;
	cfg->sys_chat_ar=arstr(0,str,cfg);
	#endif

	get_int(cfg->cdt_min_value,instream);
	get_int(cfg->max_minutes,instream);
	get_int(cfg->cdt_per_dollar,instream);
	get_str(cfg->new_pass,instream);
	get_str(cfg->new_magic,instream);
	get_str(cfg->new_sif,instream);
	get_str(cfg->new_sof,instream);
	if(!cfg->new_sof[0])		/* if output not specified, use input file */
		strcpy(cfg->new_sof,cfg->new_sif);

	/*********************/
	/* New User Settings */
	/*********************/

	get_int(cfg->new_level,instream);
	get_int(cfg->new_flags1,instream);
	get_int(cfg->new_flags2,instream);
	get_int(cfg->new_flags3,instream);
	get_int(cfg->new_flags4,instream);
	get_int(cfg->new_exempt,instream);
	get_int(cfg->new_rest,instream);
	get_int(cfg->new_cdt,instream);
	get_int(cfg->new_min,instream);
	get_str(cfg->new_xedit,instream);
	get_int(cfg->new_expire,instream);
	get_int(cfg->new_shell,instream);
	get_int(cfg->new_misc,instream);
	get_int(cfg->new_prot,instream);
	if(cfg->new_prot<SP)
		cfg->new_prot=SP;
	get_int(c,instream);
	for(i=0;i<7;i++)
		get_int(n,instream);

	/*************************/
	/* Expired User Settings */
	/*************************/

	get_int(cfg->expired_level,instream);
	get_int(cfg->expired_flags1,instream);
	get_int(cfg->expired_flags2,instream);
	get_int(cfg->expired_flags3,instream);
	get_int(cfg->expired_flags4,instream);
	get_int(cfg->expired_exempt,instream);
	get_int(cfg->expired_rest,instream);

	get_str(cfg->logon_mod,instream);
	get_str(cfg->logoff_mod,instream);
	get_str(cfg->newuser_mod,instream);
	get_str(cfg->login_mod,instream);
	if(!cfg->login_mod[0]) strcpy(cfg->login_mod,"login");
	get_str(cfg->logout_mod,instream);
	get_str(cfg->sync_mod,instream);
	get_str(cfg->expire_mod,instream);
	get_int(c,instream);

#if	1	/* temporary hack for Unix compatibility */
	strlwr (cfg->logon_mod);
	strlwr (cfg->logoff_mod);
	strlwr (cfg->newuser_mod);
	strlwr (cfg->login_mod);
	strlwr (cfg->logout_mod);
	strlwr (cfg->sync_mod);
	strlwr (cfg->expire_mod);
#endif

	for(i=0;i<224;i++)					/* unused - initialized to NULL */
		get_int(n,instream);
	for(i=0;i<256;i++)					/* unused - initialized to 0xff */
		get_int(n,instream);

	/*******************/
	/* Validation Sets */
	/*******************/

	for(i=0;i<10 && !feof(instream);i++) {
		get_int(cfg->val_level[i],instream);
		get_int(cfg->val_expire[i],instream);
		get_int(cfg->val_flags1[i],instream);
		get_int(cfg->val_flags2[i],instream);
		get_int(cfg->val_flags3[i],instream);
		get_int(cfg->val_flags4[i],instream);
		get_int(cfg->val_cdt[i],instream);
		get_int(cfg->val_exempt[i],instream);
		get_int(cfg->val_rest[i],instream);
		for(j=0;j<8;j++)
			get_int(n,instream); }

	/***************************/
	/* Security Level Settings */
	/***************************/

	for(i=0;i<100 && !feof(instream);i++) {
		get_int(cfg->level_timeperday[i],instream);
		if(cfg->level_timeperday[i]>500)
			cfg->level_timeperday[i]=500;
		get_int(cfg->level_timepercall[i],instream);
		if(cfg->level_timepercall[i]>500)
			cfg->level_timepercall[i]=500;
		get_int(cfg->level_callsperday[i],instream);
		get_int(cfg->level_freecdtperday[i],instream);
		get_int(cfg->level_linespermsg[i],instream);
		get_int(cfg->level_postsperday[i],instream);
		get_int(cfg->level_emailperday[i],instream);
		get_int(cfg->level_misc[i],instream);
		get_int(cfg->level_expireto[i],instream);
		get_int(c,instream);
		for(j=0;j<5;j++)
			get_int(n,instream); }
	if(i!=100) {
		lprintf(txt->error,offset,fname);
		lprintf("Insufficient User Level Information"
			"%d user levels read, 100 needed.",i);
		return(FALSE); }

	get_int(cfg->total_shells,instream);
	#ifdef SBBS
	if(!cfg->total_shells) {
		lprintf(txt->error,offset,fname);
		lprintf("At least one command shell must be configured.");
		return(FALSE); }
	#endif

	if(cfg->total_shells) {
		if((cfg->shell=(shell_t **)MALLOC(sizeof(shell_t *)*cfg->total_shells))==NULL)
			return allocerr(txt,offset,fname,sizeof(shell_t *)*cfg->total_shells); }
	else
		cfg->shell=NULL;

	for(i=0;i<cfg->total_shells;i++) {
		if(feof(instream)) break;
		if((cfg->shell[i]=(shell_t *)MALLOC(sizeof(shell_t)))==NULL)
			return allocerr(txt,offset,fname,sizeof(shell_t));
		memset(cfg->shell[i],0,sizeof(shell_t));

		get_alloc(&offset,cfg->shell[i]->name,40,instream);
		get_str(cfg->shell[i]->code,instream);
		strlwr(cfg->shell[i]->code);	/* temporary Unix-compatibility hack */
	#ifdef SCFG
		get_str(cfg->shell[i]->ar,instream);
	#else
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->shell[i]->ar=arstr(0,str,cfg);
	#endif
		get_int(cfg->shell[i]->misc,instream);
		for(j=0;j<8;j++)
			get_int(n,instream);
		}
	cfg->total_shells=i;


	fclose(instream);
	if(txt->readit && txt->readit[0])
		lprintf(txt->readit,fname);
	return(TRUE);
}

#endif

#ifndef NO_MSGS_CFG


/****************************************************************************/
/* Reads in MSGS.CNF and initializes the associated variables				*/
/****************************************************************************/
BOOL read_msgs_cfg(scfg_t* cfg, read_cfg_text_t* txt)
{
	char	str[256],fname[13],c;
	short	i,j,k,n;
	long	offset=0;
	FILE	*instream;

#ifndef SCFG

	sprintf(cfg->data_dir_subs,"%ssubs/",cfg->data_dir);
	prep_dir(cfg->ctrl_dir, cfg->data_dir_subs);

#endif

	strcpy(fname,"msgs.cnf");
	sprintf(str,"%s%s",cfg->ctrl_dir,fname);
	if((instream=fnopen(NULL,str,O_RDONLY))==NULL) {
		lprintf(txt->openerr,str);
		return(FALSE); }

	if(txt->reading && txt->reading[0])
		lprintf(txt->reading,fname);

	/*************************/
	/* General Message Stuff */
	/*************************/

	get_int(cfg->max_qwkmsgs,instream);
	get_int(cfg->mail_maxcrcs,instream);
	get_int(cfg->mail_maxage,instream);
	#ifdef SCFG
		get_str(cfg->preqwk_ar,instream);
	#else
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->preqwk_ar=arstr(0,str,cfg);
	#endif
	get_int(cfg->smb_retry_time,instream);	 /* odd byte */
	if(!cfg->smb_retry_time)
		cfg->smb_retry_time=30;
	for(i=0;i<235;i++)	/* NULL */
		get_int(n,instream);
	for(i=0;i<256;i++)	/* 0xff */
		get_int(n,instream);


	/******************/
	/* Message Groups */
	/******************/

	get_int(cfg->total_grps,instream);


	if(cfg->total_grps) {
		if((cfg->grp=(grp_t **)MALLOC(sizeof(grp_t *)*cfg->total_grps))==NULL)
			return allocerr(txt,offset,fname,sizeof(grp_t *)*cfg->total_grps); }
	else
		cfg->grp=NULL;


	for(i=0;i<cfg->total_grps;i++) {

		if(feof(instream)) break;
		if((cfg->grp[i]=(grp_t *)MALLOC(sizeof(grp_t)))==NULL)
			return allocerr(txt,offset,fname,sizeof(grp_t));
		memset(cfg->grp[i],0,sizeof(grp_t));

		get_alloc(&offset,cfg->grp[i]->lname,LEN_GLNAME,instream);
		get_alloc(&offset,cfg->grp[i]->sname,LEN_GSNAME,instream);

	#if !defined(SCFG) && defined(SAVE_MEMORY)	  /* Save memory */
		if(!strcmp(cfg->grp[i]->lname,cfg->grp[i]->sname) && cfg->grp[i]->sname!=scfgnulstr) {
			FREE(cfg->grp[i]->sname);
			cfg->grp[i]->sname=cfg->grp[i]->lname; }
	#endif

	#ifdef SCFG
		get_str(cfg->grp[i]->ar,instream);
	#else
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->grp[i]->ar=arstr(0,str,cfg);
	#endif
		for(j=0;j<48;j++)
			get_int(n,instream);
		}
	cfg->total_grps=i;

	/**********************/
	/* Message Sub-boards */
	/**********************/

	get_int(cfg->total_subs,instream);

	if(cfg->total_subs) {
		if((cfg->sub=(sub_t **)MALLOC(sizeof(sub_t *)*cfg->total_subs))==NULL)
			return allocerr(txt,offset,fname,sizeof(sub_t *)*cfg->total_subs); }
	else
		cfg->sub=NULL;

	for(i=0;i<cfg->total_subs;i++) {
		if(feof(instream)) break;
		if((cfg->sub[i]=(sub_t *)MALLOC(sizeof(sub_t)))==NULL)
			return allocerr(txt,offset,fname,sizeof(sub_t));
		memset(cfg->sub[i],0,sizeof(sub_t));

		get_int(cfg->sub[i]->grp,instream);
		get_alloc(&offset,cfg->sub[i]->lname,LEN_SLNAME,instream);
		get_alloc(&offset,cfg->sub[i]->sname,LEN_SSNAME,instream);

	#if !defined(SCFG) && defined(SAVE_MEMORY) /* Save memory */
		if(!strcmp(cfg->sub[i]->lname,cfg->sub[i]->sname) && cfg->sub[i]->sname!=scfgnulstr) {
			FREE(cfg->sub[i]->sname);
			cfg->sub[i]->sname=cfg->sub[i]->lname; }
	#endif

		get_alloc(&offset,cfg->sub[i]->qwkname,10,instream);

	#if !defined(SCFG) && defined(SAVE_MEMORY)	/* Save memory */
		if(!strcmp(cfg->sub[i]->qwkname,cfg->sub[i]->sname) && cfg->sub[i]->qwkname!=scfgnulstr) {
			FREE(cfg->sub[i]->qwkname);
			cfg->sub[i]->qwkname=cfg->sub[i]->sname; }
	#endif

		get_str(cfg->sub[i]->code,instream);
		strlwr(cfg->sub[i]->code); /* temporary Unix-compatibility hack */

	#ifdef SCFG
		get_str(cfg->sub[i]->data_dir,instream);
	#else
		fread(str,LEN_DIR+1,1,instream);   /* substitute data dir */
		offset+=LEN_DIR+1;
		if(str[0]) {
			prep_dir(cfg->ctrl_dir, str);
			if((cfg->sub[i]->data_dir=(char *)MALLOC(strlen(str)+1))==NULL)
				return allocerr(txt,offset,fname,strlen(str)+1);
			strcpy(cfg->sub[i]->data_dir,str); }
		else
			cfg->sub[i]->data_dir=cfg->data_dir_subs;
	#endif


	#ifdef SCFG
		get_str(cfg->sub[i]->ar,instream);
		get_str(cfg->sub[i]->read_ar,instream);
		get_str(cfg->sub[i]->post_ar,instream);
		get_str(cfg->sub[i]->op_ar,instream);
	#else
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->sub[i]->ar=arstr(0,str,cfg);
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->sub[i]->read_ar=arstr(0,str,cfg);
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->sub[i]->post_ar=arstr(0,str,cfg);
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->sub[i]->op_ar=arstr(0,str,cfg);
	#endif
		get_int(cfg->sub[i]->misc,instream);


	#ifdef SCFG
		get_str(cfg->sub[i]->tagline,instream);
	#else
		fread(str,81,1,instream);	/* substitute tagline */
		offset+=81;
		if(str[0]) {
			if((cfg->sub[i]->tagline=(char *)MALLOC(strlen(str)+1))==NULL)
				return allocerr(txt,offset,fname,strlen(str)+1);
			strcpy(cfg->sub[i]->tagline,str); }
		else
			cfg->sub[i]->tagline=cfg->qnet_tagline;
	#endif

	#ifdef SCFG
		get_str(cfg->sub[i]->origline,instream);
	#else
		fread(str,1,51,instream);	/* substitute origin line */
		offset+=51;
		if(str[0]) {
			if((cfg->sub[i]->origline=(char *)MALLOC(strlen(str)+1))==NULL)
				return allocerr(txt,offset,fname,strlen(str)+1);
			strcpy(cfg->sub[i]->origline,str); }
		else
			cfg->sub[i]->origline=cfg->origline;
	#endif

	#ifdef SCFG
		get_str(cfg->sub[i]->echomail_sem,instream);
	#else
		fread(str,1,LEN_DIR+1,instream);   /* substitute echomail semaphore */
		offset+=LEN_DIR+1;
		if(str[0]) {
			prep_path(str);
			if((cfg->sub[i]->echomail_sem=(char *)MALLOC(strlen(str)+1))==NULL)
				return allocerr(txt,offset,fname,strlen(str)+1);
			strcpy(cfg->sub[i]->echomail_sem,str); 
		} else
			cfg->sub[i]->echomail_sem=cfg->echomail_sem;
	#endif
		fread(str,1,LEN_DIR+1,instream);   /* substitute EchoMail path */
		offset+=LEN_DIR+1;
		get_int(cfg->sub[i]->faddr,instream);			/* FidoNet address */

		get_int(cfg->sub[i]->maxmsgs,instream);
		get_int(cfg->sub[i]->maxcrcs,instream);
		get_int(cfg->sub[i]->maxage,instream);
		get_int(cfg->sub[i]->ptridx,instream);
	#ifdef SBBS
		for(j=0;j<i;j++)
			if(cfg->sub[i]->ptridx==cfg->sub[j]->ptridx) {
				lprintf(txt->error,offset,fname);
				lprintf("Duplicate pointer index for subs #%d and #%d"
					,i+1,j+1);
				return(FALSE); }
	#endif

	#ifdef SCFG
		get_str(cfg->sub[i]->mod_ar,instream);
	#else
		fread(str,1,LEN_ARSTR+1,instream);
		offset+=LEN_ARSTR+1;
		cfg->sub[i]->mod_ar=arstr(0,str,cfg);
	#endif
		get_int(cfg->sub[i]->qwkconf,instream);
		get_int(c,instream);
		for(j=0;j<26;j++)
			get_int(n,instream);
		}
	cfg->total_subs=i;

	/***********/
	/* FidoNet */
	/***********/

	get_int(cfg->total_faddrs,instream);

	if(cfg->total_faddrs) {
		if((cfg->faddr=(faddr_t *)MALLOC(sizeof(faddr_t)*cfg->total_faddrs))==NULL)
			return allocerr(txt,offset,fname,sizeof(faddr_t)*cfg->total_faddrs); }
	else
		cfg->faddr=NULL;

	for(i=0;i<cfg->total_faddrs;i++)
		get_int(cfg->faddr[i],instream);

	get_str(cfg->origline,instream);
	get_str(cfg->netmail_sem,instream);
	get_str(cfg->echomail_sem,instream);
	get_str(cfg->netmail_dir,instream);
	get_str(cfg->echomail_dir,instream);
	get_str(cfg->fidofile_dir,instream);
	get_int(cfg->netmail_misc,instream);
	get_int(cfg->netmail_cost,instream);
	get_int(cfg->dflt_faddr,instream);
	for(i=0;i<28;i++)
		get_int(n,instream);

	/* Fix-up paths */
	prep_path(cfg->netmail_sem);
	prep_path(cfg->echomail_sem);
	prep_dir(cfg->netmail_dir,cfg->data_dir);
	prep_dir(cfg->echomail_dir,cfg->data_dir);
	prep_dir(cfg->fidofile_dir,cfg->data_dir);

	/**********/
	/* QWKnet */
	/**********/

	get_str(cfg->qnet_tagline,instream);

	get_int(cfg->total_qhubs,instream);

	if(cfg->total_qhubs) {
		if((cfg->qhub=(qhub_t **)MALLOC(sizeof(qhub_t *)*cfg->total_qhubs))==NULL)
			return allocerr(txt,offset,fname,sizeof(qhub_t*)*cfg->total_qhubs); }
	else
		cfg->qhub=NULL;

	for(i=0;i<cfg->total_qhubs;i++) {
		if(feof(instream)) break;
		if((cfg->qhub[i]=(qhub_t *)MALLOC(sizeof(qhub_t)))==NULL)
			return allocerr(txt,offset,fname,sizeof(qhub_t));
		memset(cfg->qhub[i],0,sizeof(qhub_t));

		get_str(cfg->qhub[i]->id,instream);
		get_int(cfg->qhub[i]->time,instream);
		get_int(cfg->qhub[i]->freq,instream);
		get_int(cfg->qhub[i]->days,instream);
		get_int(cfg->qhub[i]->node,instream);
		get_alloc(&offset,cfg->qhub[i]->call,LEN_CMD,instream);
		get_alloc(&offset,cfg->qhub[i]->pack,LEN_CMD,instream);
		get_alloc(&offset,cfg->qhub[i]->unpack,LEN_CMD,instream);
		get_int(k,instream);

		if(k) {
			if((cfg->qhub[i]->sub=(ushort *)MALLOC(sizeof(ushort)*k))==NULL)
				return allocerr(txt,offset,fname,sizeof(uint)*k);
			if((cfg->qhub[i]->conf=(ushort *)MALLOC(sizeof(ushort)*k))==NULL)
				return allocerr(txt,offset,fname,sizeof(ushort)*k);
			if((cfg->qhub[i]->mode=(char *)MALLOC(sizeof(char)*k))==NULL)
				return allocerr(txt,offset,fname,sizeof(uchar)*k); }

		for(j=0;j<k;j++) {
			if(feof(instream)) break;
			get_int(cfg->qhub[i]->conf[cfg->qhub[i]->subs],instream);
			get_int(cfg->qhub[i]->sub[cfg->qhub[i]->subs],instream);
			get_int(cfg->qhub[i]->mode[cfg->qhub[i]->subs],instream);
			if(cfg->qhub[i]->sub[cfg->qhub[i]->subs]<cfg->total_subs)
				cfg->sub[cfg->qhub[i]->sub[cfg->qhub[i]->subs]]->misc|=SUB_QNET;
			else
				continue;
			if(cfg->qhub[i]->sub[cfg->qhub[i]->subs]!=INVALID_SUB)
				cfg->qhub[i]->subs++; }
		for(j=0;j<32;j++)
			get_int(n,instream); }

	cfg->total_qhubs=i;

	for(j=0;j<32;j++)
		get_int(n,instream);

	/************/
	/* PostLink */
	/************/

	fread(str,11,1,instream);		/* Unused - used to be Site Name */
	offset+=11;
	get_int(cfg->sys_psnum,instream);	/* Site Number */
	get_int(cfg->total_phubs,instream);

	if(cfg->total_phubs) {
		if((cfg->phub=(phub_t **)MALLOC(sizeof(phub_t *)*cfg->total_phubs))==NULL)
			return allocerr(txt,offset,fname,sizeof(phub_t*)*cfg->total_phubs); }
	else
		cfg->phub=NULL;

	for(i=0;i<cfg->total_phubs;i++) {
		if(feof(instream)) break;
		if((cfg->phub[i]=(phub_t *)MALLOC(sizeof(phub_t)))==NULL)
			return allocerr(txt,offset,fname,sizeof(phub_t));
		memset(cfg->phub[i],0,sizeof(phub_t));
	#ifdef SCFG
		get_str(cfg->phub[i]->name,instream);
	#else
		fread(str,11,1,instream);
		offset+=11;
	#endif
		get_int(cfg->phub[i]->time,instream);
		get_int(cfg->phub[i]->freq,instream);
		get_int(cfg->phub[i]->days,instream);
		get_int(cfg->phub[i]->node,instream);
		get_alloc(&offset,cfg->phub[i]->call,LEN_CMD,instream);
		for(j=0;j<32;j++)
			get_int(n,instream); }

	cfg->total_phubs=i;

	get_str(cfg->sys_psname,instream);	/* Site Name */

	for(j=0;j<32;j++)
		get_int(n,instream);

	/* Internet */

	get_str(cfg->sys_inetaddr,instream); /* Internet address */
	get_str(cfg->inetmail_sem,instream);
	prep_path(cfg->inetmail_sem);
	get_int(cfg->inetmail_misc,instream);
	get_int(cfg->inetmail_cost,instream);

	fclose(instream);
	if(txt->readit && txt->readit[0])
		lprintf(txt->readit,fname);
	return(TRUE);
}

#endif

void free_node_cfg(scfg_t* cfg)
{
	int i;

	FREE_AR(cfg->node_ar);

	if(cfg->mdm_result!=NULL) {
		for(i=0;i<cfg->mdm_results;i++) {
			FREE_ALLOC(cfg->mdm_result[i].str);
		}
		FREE_AND_NULL(cfg->mdm_result);
	}

	for(i=0;i<10;i++) { 						/* WFC 0-9 DOS commands */
		FREE_ALLOC(cfg->wfc_cmd[i]); 
	}
	for(i=0;i<12;i++) { 						/* WFC F1-F12 shrinking DOS cmds */
		FREE_ALLOC(cfg->wfc_scmd[i]); 
	}
}

void free_main_cfg(scfg_t* cfg)
{
	int i;

	FREE_AR(cfg->sys_chat_ar);

	if(cfg->node_path!=NULL) {
		for(i=0;i<cfg->sys_nodes;i++)
			FREE_AND_NULL(cfg->node_path[i]);
		FREE_AND_NULL(cfg->node_path);
	}
	if(cfg->shell!=NULL) {
		for(i=0;i<cfg->total_shells;i++) {
			FREE_AR(cfg->shell[i]->ar);
			FREE_ALLOC(cfg->shell[i]->name);
			FREE_AND_NULL(cfg->shell[i]);
		}
		FREE_AND_NULL(cfg->shell);
	}
}

void free_msgs_cfg(scfg_t* cfg)
{
	int i;

	FREE_AR(cfg->preqwk_ar);
	if(cfg->grp!=NULL) {
		for(i=0;i<cfg->total_grps;i++) {
			FREE_AR(cfg->grp[i]->ar);
			FREE_ALLOC(cfg->grp[i]->lname);
			FREE_ALLOC(cfg->grp[i]->sname);
			FREE_AND_NULL(cfg->grp[i]);
		}
		FREE_AND_NULL(cfg->grp);
	}

	if(cfg->sub!=NULL) {
		for(i=0;i<cfg->total_subs;i++) {
#ifndef SCFG
			if(cfg->sub[i]->data_dir!=cfg->data_dir_subs)
				FREE_AND_NULL(cfg->sub[i]->data_dir);
			if(cfg->sub[i]->tagline!=cfg->qnet_tagline)
				FREE_AND_NULL(cfg->sub[i]->tagline);
			if(cfg->sub[i]->origline!=cfg->origline)
				FREE_AND_NULL(cfg->sub[i]->origline);
			if(cfg->sub[i]->echomail_sem!=cfg->echomail_sem)
				FREE_AND_NULL(cfg->sub[i]->echomail_sem);
#endif
			FREE_AR(cfg->sub[i]->ar);
			FREE_AR(cfg->sub[i]->read_ar);
			FREE_AR(cfg->sub[i]->post_ar);
			FREE_AR(cfg->sub[i]->op_ar);
			FREE_AR(cfg->sub[i]->mod_ar);
			FREE_ALLOC(cfg->sub[i]->lname);
			FREE_ALLOC(cfg->sub[i]->sname);
			FREE_ALLOC(cfg->sub[i]->qwkname);
			FREE_AND_NULL(cfg->sub[i]);
		}
		FREE_AND_NULL(cfg->sub);
	}

	FREE_AND_NULL(cfg->faddr);
	cfg->total_faddrs=0;

	if(cfg->qhub!=NULL) {
		for(i=0;i<cfg->total_qhubs;i++) {
			FREE_ALLOC(cfg->qhub[i]->call);
			FREE_ALLOC(cfg->qhub[i]->pack);
			FREE_ALLOC(cfg->qhub[i]->unpack);
			FREE_AND_NULL(cfg->qhub[i]->mode);
			FREE_AND_NULL(cfg->qhub[i]->conf);
			FREE_AND_NULL(cfg->qhub[i]->sub);
			FREE_AND_NULL(cfg->qhub[i]); }
		FREE_AND_NULL(cfg->qhub);
	}

	if(cfg->phub!=NULL) {
		for(i=0;i<cfg->total_phubs;i++) {
			FREE_ALLOC(cfg->phub[i]->call);
			FREE_AND_NULL(cfg->phub[i]);
		}
		FREE_AND_NULL(cfg->phub);
	}
}

/************************************************************/
/* Create data and sub-dirs off data if not already created */
/************************************************************/
void make_data_dirs(scfg_t* cfg)
{
	char str[MAX_PATH+1];

	md(cfg->data_dir);
	sprintf(str,"%ssubs",cfg->data_dir);
	md(str);
	sprintf(str,"%sdirs",cfg->data_dir);
	md(str);
	sprintf(str,"%stext",cfg->data_dir);
	md(str);
	sprintf(str,"%smsgs",cfg->data_dir);
	md(str);
	sprintf(str,"%suser",cfg->data_dir);
	md(str);
	sprintf(str,"%suser/ptrs",cfg->data_dir);
	md(str);
	sprintf(str,"%slogs",cfg->data_dir);
	md(str);
	sprintf(str,"%sqnet",cfg->data_dir);
	md(str);
	sprintf(str,"%sfile",cfg->data_dir);
	md(str);
}