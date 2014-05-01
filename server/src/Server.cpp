// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Server.h"
#include "../../protocol.h"


#define MAX_VIDEOBUFFERS 16
#define MAX_AUDIOBUFFERS 32
#define MAX_UTTERANCEBUFFERS 32
#define MAX_FACEBUFFERS 32
#define MAX_TEXTBUFFERS 32
#define MAX_SPEECHBUFFERS 32
#define MAX_LIPSYNCBUFFERS 32


Server::Server() : Task(),lfd(-1),cfd(-1),
	videopool(MAX_VIDEOBUFFERS),
	audiopool(MAX_AUDIOBUFFERS),
	utterancepool(MAX_UTTERANCEBUFFERS),
	facepool(MAX_FACEBUFFERS),
	textpool(MAX_TEXTBUFFERS),
	speechpool(MAX_SPEECHBUFFERS),
	lipsyncpool(MAX_LIPSYNCBUFFERS),
	servos(*this,HEAD_PORTNAME,BODY_PORTNAME),

	stt(*this,utterancepool),
	microphone(*this,stt,audiopool,MICROPHONE_DEVICE),
	speaker(*this,SPEAKER_DEVICE),
	tts(*this,speaker,servos,microphone,speechpool,lipsyncpool),
	vision(*this,facepool),
	camera(*this,videopool,CAMERA_DEVICE),

	compressed(0),
	compressed_size(0)
{
	lfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(lfd == -1)
	{
		printf("Cannot create client connection listener socket, aborting\n");
		exit(1);
	}
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(oz::OZ_PORT);
	sa.sin_addr.s_addr = 0;
	if(bind(lfd,(sockaddr*)&sa,sizeof(sockaddr_in)) != 0)
	{
		printf("Cannot bind client connection listener socket, aborting\n");
		exit(1);
	}
	listen(lfd,5);

	compressed = new unsigned char[VIDEO_FRAME];  // max. size is full frame (est.)
	image_y = (JSAMPROW* )malloc(sizeof(JSAMPROW) * VIDEO_YSIZE + 16);
    image_u = (JSAMPROW* )malloc(sizeof(JSAMPROW) * VIDEO_YSIZE + 16);
    image_v = (JSAMPROW* )malloc(sizeof(JSAMPROW) * VIDEO_YSIZE + 16);

	servos.Start();
	stt.Start();
	microphone.Start();
	speaker.Start();
	tts.Start();
	vision.Start();
	camera.Start();

	printf("Ready!\n");
}


Server::~Server()
{
	if(cfd != -1)
		close(cfd);

	delete[] compressed;

	if(lfd != -1)
		close(lfd);

	camera.Post(ITM_EXIT);
	vision.Post(ITM_EXIT);
	tts.Post(ITM_EXIT);
	speaker.Post(ITM_EXIT);
	microphone.Post(ITM_EXIT);
	stt.Post(ITM_EXIT);
	servos.Post(ITM_EXIT);
}


void Server::Step()
{
	struct pollfd pfd[3];

	pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	pfd[1].fd = lfd;
	pfd[1].events = POLLIN;
	pfd[1].revents = 0;

	if(cfd != -1)
	{
		pfd[2].fd = cfd;
		pfd[2].events = POLLIN;
		pfd[2].revents = 0;

		poll(pfd,3,-1);

		if(pfd[2].revents & POLLIN)
		{
			unsigned char dummy;
			if(recv(cfd,&dummy,1,MSG_PEEK) == 0)
			{
				printf("Client connection lost\n");

				close(cfd);
				cfd = -1;
			}
			else
			{
				unsigned int msg = ReceiveDword();
				switch(msg)
				{
					case oz::MSG_REQUEST_VERSION:  // request particular version of the API, dword = version (0x01000000 = 1.0), robot returns version as answer, followed by name and long name
						{
							ReceiveDword();  // discard result, there is only one version
							SendDword(API_VERSION);
							SendString(ROBOT_NAME);
							SendString(ROBOT_LONG_NAME);
						}
						break;

					case oz::MSG_RESET:  // reset robot
						servos.Post(ITM_RESET,0);
						break;

					case oz::MSG_SPEAK:  // speak, text = utterance
						{
							textbuffer* text = textpool.get();
							ReceiveString(text->data,MAX_TEXT_CHARACTERS);
							tts.Post(ITM_TEXT,text);
						}
						break;

					case oz::MSG_VOICE_GENDER_LANGUAGE:  // set voice/gender/language, dword = VCL_* (default = VCL_DEFAULT)
						{
							int vgl = ReceiveDword();
							printf("MSG_VOICE_GENDER_LANGUAGE %d\n",vgl);
							// TODO
						}
						break;

					case oz::MSG_VOICE_VOLUME:  // set voice volume, float = volume (0..1, default = 1)
						{
							float volume = ReceiveFloat();
							printf("MSG_VOICE_VOLUME %f\n",volume);
							// TODO
						}
						break;

					case oz::MSG_VOICE_PITCH:  // set voice pitch, float = pitch (0..2, default = 1)
						{
							float pitch = ReceiveFloat();
							printf("MSG_VOICE_PITCH %f\n",pitch);
							// TODO
						}
						break;

					case oz::MSG_GESTURE:  // manually trigger gesture, dword = GESTURE_*
						{
							int gesture = ReceiveDword();
							servos.Post(ITM_GESTURE,*(void**)&gesture);
						}
						break;

					case oz::MSG_GESTURE_WEIGHT:  // set gesture weight, float = weight (0..1, default = 1)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_GESTURE_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_EXPRESSION:  // set facial expression, dword = EXPRESSION_* (default = EXPRESSION_NEUTRAL)
						{
							int expression = ReceiveDword();
							servos.Post(ITM_EXPRESSION,*(void**)&expression);
						}
						break;

					case oz::MSG_EXPRESSION_WEIGHT:  // set facial expression weight, float = weight (0..1, default = 1)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_EXPRESSION_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_AUTO_BLINK_SPEED:  // set automatic blink speed, float = speed in Hz (0..10, default = 0, no blinking)
						{
							float speed = ReceiveFloat();
							servos.Post(ITM_AUTO_BLINK_SPEED,*(void**)&speed);
						}
						break;

					case oz::MSG_RANDOM_HEAD_WEIGHT:  // set random head motion weight, float = weight (0..1, default = 0)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_RANDOM_HEAD_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_RANDOM_EYES_WEIGHT:  // set random eyes motion weight, float = weight (0..1, default = 0)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_RANDOM_EYES_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_RANDOM_FACE_WEIGHT:  // set random face motion weight, float = weight (0..1, default = 0)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_RANDOM_FACE_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_SERVO:  // set servo position, dword = id, float = position (0..1, default = 0)
						{
							int id = ReceiveDword();
							float pos = ReceiveFloat();
							printf("MSG_SERVO %d %f\n",id,pos);
							// TODO: set servo
						}
						break;

					case oz::MSG_ALL_SERVOS:  // set all servo positions, { float = position (0..1, default = 0) }
						{
							float pos[oz::MAX_SERVOS];
							printf("MSG_ALL_SERVOS ");
							for(int i = 0; i < oz::MAX_SERVOS; i++)
								printf("%f ",pos[i]);
							printf("\n");
							// TODO: set all servos
						}
						break;

					case oz::MSG_DIRECT_WEIGHT:  // set direct servo control weight, float = weight (0..1, default = 0)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_DIRECT_WEIGHT,*(void**)&weight);
						}
						break;

					case oz::MSG_LOOKAT:  // set lookat target, dword = LOOKAT_* (default = LOOKAT_FORWARD)
						{
							int lookat = ReceiveDword();
							servos.Post(ITM_LOOKAT,*(void**)&lookat);
						}
						break;

					case oz::MSG_LOOKAT_WEIGHT:  // set lookat weight, float = weight (0..1, default = 0)
						{
							float weight = ReceiveFloat();
							servos.Post(ITM_LOOKAT_WEIGHT,*(void**)&weight);
						}
						break;
				}
			}
		}
	}
	else
		poll(pfd,2,-1);

	if(pfd[0].revents & POLLIN)
		FlushMessages();

	if(pfd[1].revents & POLLIN)
	{
		int fd = accept(lfd,0,0);
		if(cfd != -1)
		{
			printf("Client connection refused\n");

			close(fd);
		}
		else
		{
			printf("Client connection accepted\n");

			cfd = fd;
		}
	}
}


void Server::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_SPEECH_DONE:  // from tts
			if(cfd != -1)
				SendDword(oz::MSG_SPEAK_DONE);
			break;

		case ITM_GESTURE:  // from servos
			// TODO
			break;

		case ITM_GESTURE_DONE:  // from servos
			// TODO
			break;

		case ITM_VIDEO:  // from camera
			{
				videobuffer* video = (videobuffer*)data;
				if(cfd != -1)
				{
					// compress to jpeg and send over
				    struct jpeg_compress_struct cinfo;
				    struct jpeg_error_mgr jerr;
				    cinfo.err = jpeg_std_error(&jerr);
				    jpeg_create_compress(&cinfo);
				    compressed_size = 0;
				    jpeg_mem_dest(&cinfo,&compressed,&compressed_size);
				    cinfo.image_width = VIDEO_XSIZE;
				    cinfo.image_height = VIDEO_YSIZE;
				    cinfo.input_components = 3;
				    cinfo.in_color_space = JCS_YCbCr;
				    jpeg_set_defaults(&cinfo);
					cinfo.do_fancy_downsampling = FALSE;
					cinfo.dct_method = JDCT_FASTEST;
					cinfo.smoothing_factor = 0;
				    jpeg_set_quality(&cinfo,32,TRUE);
			        cinfo.raw_data_in = TRUE;
					cinfo.comp_info[0].h_samp_factor = 2;
					cinfo.comp_info[0].v_samp_factor = 2;
					cinfo.comp_info[1].h_samp_factor = 1;
					cinfo.comp_info[1].v_samp_factor = 1;
					cinfo.comp_info[2].h_samp_factor = 1;
					cinfo.comp_info[2].v_samp_factor = 1;
		            jpeg_start_compress(&cinfo,TRUE);
		            for(int i = 0; i < (VIDEO_YSIZE + 16) / 2; i++)
		            {
		                image_y[i * 2] = video->data + i * 2 * VIDEO_XSIZE;
		                image_y[i * 2 + 1] = video->data + (i * 2 + 1) * VIDEO_XSIZE;
		                image_u[i] = video->data + VIDEO_XSIZE * VIDEO_YSIZE + i * VIDEO_XSIZE / 2;
		                image_v[i] = video->data + (VIDEO_XSIZE * VIDEO_YSIZE * 5) / 4 + i * VIDEO_XSIZE / 2;
		            }
		            for(int i = 0; i < VIDEO_YSIZE / (2 * DCTSIZE); i++)
				    {
		            	JSAMPARRAY image[3];
				        image[0] = &image_y[i * 2 * DCTSIZE];
				        image[1] = &image_u[i * DCTSIZE];
				        image[2] = &image_v[i * DCTSIZE];
				        jpeg_write_raw_data(&cinfo,image,2 * DCTSIZE);
		            }
		            jpeg_finish_compress(&cinfo);
				    jpeg_destroy_compress(&cinfo);
				    //printf("sending video (%d bytes)\n",compressed_size);
				    SendDword(oz::MSG_VIDEO_FRAME);
				    SendDword(compressed_size);
				    Send(compressed,compressed_size);
				}
				vision.Post(ITM_VIDEO,video);
			}
			break;

		case ITM_FACES:  // from vision
			{
				facebuffer* faces = (facebuffer*)data;
				if(cfd != -1)
				{
					printf("sending faces (%d)\n",faces->faces);
					SendDword(oz::MSG_FACES);
					SendDword(faces->faces);
					for(int i = 0; i < faces->faces; i++)
					{
						SendFloat(1.0f);

						// calculate center
						int2 p(faces->face[i].rect.o);
						p.x += faces->face[i].rect.s.x / 2;
						p.y += faces->face[i].rect.s.y / 2;

						// send normalized to -1..1
	                    SendFloat(-1.0f + 2.0f * (float)p.x / (float)VIDEO_XSIZE);
	                    SendFloat(-1.0f + 2.0f * (float)p.y / (float)VIDEO_YSIZE);

	                    // get size
	                    p.x = faces->face[i].rect.s.x;
	                    p.y = faces->face[i].rect.s.y;

	                    // send normalized
	                    SendFloat((float)p.x / (float)VIDEO_XSIZE);
	                    SendFloat((float)p.y / (float)VIDEO_YSIZE);

	                    // send identity
	                    SendString(faces->face[i].identity);
					}
				}
				faces->release();
			}
			break;

		case ITM_UTTERANCE:  // from stt
			{
				utterancebuffer* utterances = (utterancebuffer*)data;
				if(cfd != -1)
				{
					printf("sending utterances (%d)\n",utterances->options);
					//printf("Server: sending MSG_UTTERANCE to client\n");
					SendDword(oz::MSG_UTTERANCE);
					SendDword(utterances->options);
					for(int i = 0; i < utterances->options; i++)
					{
						SendFloat(utterances->option[i].confidence);
						SendString(utterances->option[i].data);
					}
				}
				utterances->release();
			}
			break;
	}
}


#define CHUNK_SIZE 32768


int Server::Send(unsigned char* buffer,int length)
{
    int rem = length;
    int total = 0;
    while(rem > 0)
    {
        int chunk = rem;
        if(chunk > CHUNK_SIZE)
            chunk = CHUNK_SIZE;
        int bytes = ::send(cfd,&(buffer[total]),chunk,0);
        if(bytes < 0)
            return total;
        rem -= bytes;
        total -= bytes;
    }
    return total;
}


int Server::Receive(unsigned char* buffer,int length)
{
    int rem = length;
    int total = 0;
    while(rem > 0)
    {
        int chunk = rem;
        if(chunk > CHUNK_SIZE)
            chunk = CHUNK_SIZE;
        int bytes = ::recv(cfd,&(buffer[total]),chunk,0);
        if(bytes == -1)
            return total;
        rem -= bytes;
        total -= bytes;
    }
    return total;
}


void Server::SendByte(unsigned char b)
{
    Send(&b,1);
}


unsigned char Server::ReceiveByte()
{
    unsigned char b;
    Receive(&b,1);
    return b;
}


void Server::SendWord(unsigned short w)
{
    unsigned char b[2];
    b[0] = w >> 8;
    b[1] = w & 255;
    Send(b,2);
}


unsigned short Server::ReceiveWord()
{
    unsigned char b[2];
    Receive(b,2);
    return (b[0] << 8) | b[1];
}


void Server::SendDword(unsigned int d)
{
    unsigned char b[4];
    b[0] = d >> 24;
    b[1] = (d >> 16) & 255;
    b[2] = (d >> 8) & 255;
    b[3] = d & 255;
    Send(b,4);
}


unsigned int Server::ReceiveDword()
{
    unsigned char b[4];
    Receive(b,4);
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}


void Server::SendFloat(float f)
{
    unsigned int d = *(unsigned int*)&f;
    unsigned char b[4];
    b[0] = d >> 24;
    b[1] = (d >> 16) & 255;
    b[2] = (d >> 8) & 255;
    b[3] = d & 255;
    Send(b,4);
}


float Server::ReceiveFloat()
{
    unsigned char b[4];
    Receive(b,4);
    unsigned int d = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
    return *(float*)&d;
}


void Server::SendDouble(double d)
{
    unsigned long long int a = *(unsigned long long int*)&d;
    unsigned char b[8];
    b[0] = a >> 56;
    b[1] = (a >> 48) & 255;
    b[2] = (a >> 40) & 255;
    b[3] = (a >> 32) & 255;
    b[4] = (a >> 24) & 255;
    b[5] = (a >> 16) & 255;
    b[6] = (a >> 8) & 255;
    b[7] = a & 255;
    Send(b,8);
}


double Server::ReceiveDouble()
{
    unsigned char b[8];
    Receive(b,8);
    unsigned long long int a = ((unsigned long long int)b[0] << 56) | ((unsigned long long int)b[1] << 48) | ((unsigned long long int)b[2] << 40) | ((unsigned long long int)b[3] << 32) | ((unsigned long long int)b[4] << 24) | ((unsigned long long int)b[5] << 16) | ((unsigned long long int)b[6] << 8) | (unsigned long long int)b[7];
    return *(double*)&a;
}


void Server::SendString(char* s)
{
    SendWord(strlen(s));
    unsigned char* us = (unsigned char*)s;
    Send(us,strlen(s));
}


void Server::ReceiveString(char* s,int max)
{
    int length = ReceiveWord();
    if(length > max - 1)
    {
        Receive((unsigned char*)s,max - 1);
        s[max - 1] = 0;
        unsigned char dummy[65536];
        Receive(dummy,length - max + 1);
    }
    else
    {
        Receive((unsigned char*)s,length);
        s[length] = 0;
    }
}
