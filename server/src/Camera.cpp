// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Camera.h"
#include "Server.h"
#include "Vision.h"


Camera::Camera(Server& aserver,pool<videobuffer>& avideopool,char* name) : Task(),server(aserver),videopool(avideopool),cfd(-1),type(VIDEO_UNKNOWN)
{
	// open device
	cfd = open(name,O_RDWR);
	if(cfd == -1)
	{
		printf("Cannot open device (%s), so no video available\n",name);
		return;
	}

	// get capabilities
	struct v4l2_capability cap;
	if(ioctl(cfd,VIDIOC_QUERYCAP,&cap) == -1)
	{
		printf("Cannot query camera capabilities, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// verify capture capability
	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("Camera does not support capturing, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// verify streaming capability
	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("Camera does not support streaming, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// find camera input on device
	int ninput = -1;
	v4l2_input input;
	input.index = 0;
	while(true)
	{
		if(ioctl(cfd,VIDIOC_ENUMINPUT,&input) == -1)
		{
			if(errno == EINVAL)
				break;
			printf("Cannot enumerate camera video inputs, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
		if((ninput == -1) && (input.type & V4L2_INPUT_TYPE_CAMERA))
			ninput = input.index;
		input.index++;
	}
	if(ninput == -1)
	{
		printf("Camera has no video input, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// enumerate image formats
	v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	while(true)
	{
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(ioctl(cfd,VIDIOC_ENUM_FMT,&fmtdesc) == -1)
		{
			if(errno == EINVAL)
				break;
			printf("Cannot enumerate camera image formats, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
		fmtdesc.index++;
	}

	// select video input
	if(ioctl(cfd,VIDIOC_S_INPUT,&ninput) == -1)
	{
		printf("Cannot select video input on camera, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// enumerate inputs
	input.index = ninput;
	if(ioctl(cfd,VIDIOC_ENUMINPUT,&input) == -1)
	{
		printf("Cannot enumerate camera inputs, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	v4l2_standard standard;
	if(input.std)
	{
		// enumerate standards
		standard.index = 0;
		while(true)
		{
			if(ioctl(cfd,VIDIOC_ENUMSTD,&standard) == -1)
			{
				if(errno == EINVAL)
					break;
				printf("Cannot enumerate camera standards, so no video available\n");
				close(cfd);
				cfd = -1;
				return;
			}
			standard.index++;
		}

		// get current standard
		v4l2_std_id std_id;
		if(ioctl(cfd,VIDIOC_G_STD,&std_id) == -1)
		{
			printf("Cannot get current camera standard, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}

		// match standards
		standard.index = 0;
		while(true)
		{
			if(ioctl(cfd,VIDIOC_ENUMSTD,&standard) == -1)
			{
				printf("Cannot enumerate camera standards, so no video available\n");
				close(cfd);
				cfd = -1;
				return;
			}
			if(standard.id == std_id)
				break;
			standard.index++;
		}
	}

	// get data format
	v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(cfd,VIDIOC_G_FMT,&format) == -1)
	{
		printf("Cannot get camera data format, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// get streaming parameters
	v4l2_streamparm streamparm;
	memset(&streamparm,0,sizeof(v4l2_streamparm));
	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(cfd,VIDIOC_G_PARM,&streamparm) == -1)
	{
		printf("Cannot get camera streaming parameters, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// set data format
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = VIDEO_XSIZE;
	format.fmt.pix.height = VIDEO_YSIZE;

	// first try YUV420
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
	type = VIDEO_YUV420;
	if((ioctl(cfd,VIDIOC_S_FMT,&format) == -1) || (format.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420))
	{
		// then try YUYV
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		type = VIDEO_YUYV;
		if((ioctl(cfd,VIDIOC_S_FMT,&format) == -1) || (format.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV))
		{
			// then try BA81
			format.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
			type = VIDEO_BA81;
			if((ioctl(cfd,VIDIOC_S_FMT,&format) == -1) || (format.fmt.pix.pixelformat != V4L2_PIX_FMT_SBGGR8))
			{
				unsigned long d = format.fmt.pix.pixelformat;
				printf("Camera does not YUV420, YUYV or BA81, \"%c%c%c%c\" suggested instead, so no video available\n",(int)(d & 255),(int)((d >> 8) & 255),(int)((d >> 16) & 255),(int)(d >> 24));
				close(cfd);
				cfd = -1;
				return;
			}
		}
	}
	if((format.fmt.pix.width != VIDEO_XSIZE) || (format.fmt.pix.height != VIDEO_YSIZE))
	{
		printf("Camera does not support %dx%d, so no video available\n",VIDEO_XSIZE,VIDEO_YSIZE);
		close(cfd);
		cfd = -1;
		return;
	}

	// set streaming parameters
	if(streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
	{
		streamparm.parm.capture.timeperframe.numerator = 1;
		streamparm.parm.capture.timeperframe.denominator = VIDEO_FPS;
		if(ioctl(cfd,VIDIOC_S_PARM,&streamparm) == -1)
		{
			printf("Cannot set camera streaming parameters, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
		if(ioctl(cfd,VIDIOC_G_PARM,&streamparm) == -1)
		{
			printf("Cannot get camera streaming parameters, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
		if(streamparm.parm.capture.timeperframe.denominator != VIDEO_FPS)
		{
			printf("Frame rate not supported on camera (%dHz wanted, but %dHz suggested), so no video available\n",VIDEO_FPS,streamparm.parm.capture.timeperframe.denominator);
			close(cfd);
			cfd = -1;
			return;
		}
	}

	// initialize memory mapping I/O
	v4l2_requestbuffers reqbuf;
	memset(&reqbuf,0,sizeof(v4l2_requestbuffers));
	reqbuf.count = MAX_BUFFERS;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	if(ioctl(cfd,VIDIOC_REQBUFS,&reqbuf) == -1)
	{
		printf("Unable to request mapped buffers, so no video available\n");
		close(cfd);
		cfd = -1;
		return;
	}

	// we want at least five buffers
	if(reqbuf.count < MAX_BUFFERS)
	{
		// you may need to free the buffers here
		printf("Not enough mapped buffers available (atleast %d needed), so no video available\n",MAX_BUFFERS);
		close(cfd);
		cfd = -1;
		return;
	}

	// map buffers in user address space
	for(int i = 0; i < MAX_BUFFERS; i++)
	{
		v4l2_buffer buf;
		memset(&buf,0,sizeof(v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.index = i;
		if(ioctl(cfd,VIDIOC_QUERYBUF,&buf) == -1)
		{
			printf("Cannot map buffers, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
		buffer[i].length = buf.length;
		if((buffer[i].mapped = (unsigned char*)mmap(0,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,cfd,buf.m.offset)) == MAP_FAILED)
		{
			printf("Cannot map buffers, so no video available\n");
			close(cfd);
			cfd = -1;
			return;
		}
	}

	printf("Initialized camera\n");
}


Camera::~Camera()
{
	// memory-mapped frame buffers will leak here
	if(cfd != -1)
		close(cfd);
}


void ConvertToYUV(unsigned char* dest,unsigned char* src,int type)
{
	switch(type)
	{
		case VIDEO_YUV420:
			memcpy(dest,src,VIDEO_FRAME);
			break;

		case VIDEO_YUYV:
			{
				// convert YUYV frame to planar YUV420 format
				int stride = VIDEO_XSIZE * 2;
				for(int i = 0; i < VIDEO_YSIZE / 2; i++)
					for(int k = 0; k < VIDEO_XSIZE / 2; k++)
					{
						// top two pixels
						unsigned char y0 = src[i * 2 * stride + k * 4];
						unsigned char u01 = src[i * 2 * stride + k * 4 + 1];
						unsigned char y1 = src[i * 2 * stride + k * 4 + 2];
						unsigned char v01 = src[i * 2 * stride + k * 4 + 3];

						// bottom two pixels
						unsigned char y2 = src[(i * 2 + 1) * stride + k * 4];
						unsigned char u23 = src[(i * 2 + 1) * stride + k * 4 + 1];
						unsigned char y3 = src[(i * 2 + 1) * stride + k * 4 + 2];
						unsigned char v23 = src[(i * 2 + 1) * stride + k * 4 + 3];

						// combine U and V
						unsigned char u = (unsigned char)(((int)u01 + (int)u23) / 2);
						unsigned char v = (unsigned char)(((int)v01 + (int)v23) / 2);

						// write to planar format
						dest[i * 2 * VIDEO_XSIZE + k * 2] = y0;
						dest[i * 2 * VIDEO_XSIZE + k * 2 + 1] = y1;
						dest[(i * 2 + 1) * VIDEO_XSIZE + k * 2] = y2;
						dest[(i * 2 + 1) * VIDEO_XSIZE + k * 2 + 1] = y3;
						dest[VIDEO_XSIZE * VIDEO_YSIZE + i * VIDEO_XSIZE / 2 + k] = u;
						dest[VIDEO_XSIZE * VIDEO_YSIZE + VIDEO_XSIZE * VIDEO_YSIZE / 4 + i * VIDEO_XSIZE / 2 + k] = v;
					}
			}
			break;

		case VIDEO_BA81:
			for(int y = 0; y < VIDEO_YSIZE / 2; y++)
				for(int x = 0; x < VIDEO_XSIZE / 2; x++)
				{
					// invert the coordinates (BA81 is bottom-up/right-left)
					int revy = VIDEO_YSIZE / 2 - y - 1;
					int revx = VIDEO_XSIZE / 2 - x - 1;

					// get pointer to lower-right pixel of 2x2 block
					unsigned char* ptr = &(src[revy * 2 * VIDEO_XSIZE + revx * 2]);

					// get samples
					int m0 = ptr[-VIDEO_XSIZE - 1];
					int m1 = ptr[-VIDEO_XSIZE];
					int m2 = ptr[-VIDEO_XSIZE + 1];
					int m4 = ptr[-1];
					int b3 = ptr[0];  // lower-right pixel (blue)
					int g2 = ptr[1];  // lower-left pixel (green)
					int m7 = ptr[2];
					int m8 = ptr[VIDEO_XSIZE - 1];
					int g1 = ptr[VIDEO_XSIZE];  // upper-right pixel (green) 
					int r0 = ptr[VIDEO_XSIZE + 1];  // upper-left pixel (red)
					int m11 = ptr[VIDEO_XSIZE + 2];
					int m13 = ptr[2 * VIDEO_XSIZE];
					int m14 = ptr[2 * VIDEO_XSIZE + 1];
					int m15 = ptr[2 * VIDEO_XSIZE + 2];

					// interpolate
					int g0,b0,r1,b1,r2,b2,r3,g3;
					if(y == 0)
					{
						b1 = b3;
						r2 = (m2 + r0) / 2;
						if(x == 0)
						{
							g0 = (g2 + g1) / 2;
							b0 = b3;
							r1 = (m8 + r0) / 2;
							b2 = b3;
							r3 = (m0 + m2 + m8 + r0) / 4;
							g3 = (m1 + m4 + g2 + g1) / 4;
						}
						else if(x < VIDEO_XSIZE / 2 - 1)
						{
							g0 = (g2 + g1 + m11) / 3;
							b0 = (b3 + m7) / 2;
							r1 = (m8 + r0) / 2;
							b2 = (b3 + m7) / 2;
							r3 = (m0 + m2 + m8 + r0) / 4;
							g3 = (m1 + m4 + g2 + g1) / 4;
						}
						else
						{
							g0 = (g2 + g1 + m11) / 3;
							b0 = (b3 + m7) / 2;
							r1 = r0;
							b2 = (b3 + m7) / 2;
							r3 = r2;
							g3 = (m1 + g2 + g1) / 3;
						}
					}
					else if(y < VIDEO_YSIZE / 2 - 1)
					{
						b1 = (b3 + m13) / 2;
						r2 = (m2 + r0) / 2;
						if(x == 0)
						{
							g0 = (g2 + g1 + m14) / 3;
							b0 = b1;
							r1 = (m8 + r0) / 2;
							b2 = b3;
							r3 = (m0 + m2 + m8 + r0) / 4;
							g3 = (m1 + m4 + g2 + g1) / 4;
						}
						else if(x < VIDEO_XSIZE / 2 - 1)
						{
							g0 = (g2 + g1 + m11 + m14) / 4;
							b0 = (b3 + m7 + m13 + m15) / 4;
							r1 = (m8 + r0) / 2;
							b2 = (b3 + m7) / 2;
							r3 = (m0 + m2 + m8 + r0) / 4;
							g3 = (m1 + m4 + g2 + g1) / 4;
						}
						else
						{
							g0 = (g2 + g1 + m11 + m14) / 4;
							b0 = (b3 + m7 + m13 + m15) / 4;
							r1 = r0;
							b2 = (b3 + m7) / 2;
							r3 = r2;
							g3 = (m1 + g2 + g1) / 3;
						}
					}
					else
					{
						b1 = (b3 + m13) / 2;
						r2 = r0;
						if(x == 0)
						{
							g0 = (g2 + g1 + m14) / 3;
							b0 = b1;
							r1 = (m8 + r0) / 2;
							b2 = b3;
							r3 = r1;
							g3 = (m4 + g2 + g1) / 3;
						}
						else if(x < VIDEO_XSIZE / 2 - 1)
						{
							g0 = (g2 + g1 + m11 + m14) / 4;
							b0 = (b3 + m7 + m13 + m15) / 4;
							r1 = (m8 + r0) / 2;
							b2 = (b3 + m7) / 2;
							r3 = r1;
							g3 = (m4 + g2 + g1) / 3;
						}
						else
						{
							g0 = (g2 + g1 + m11 + m14) / 4;
							b0 = (b3 + m7 + m13 + m15) / 4;
							r1 = r0;
							b2 = (b3 + m7) / 2;
							r3 = r0;
							g3 = (g2 + g1) / 2;
						}
					}

					// convert block to YUV
					int y0 = 16 + ((66 * r0 + 129 * g0 + 25 * b0 + 128) >> 8);
					int u0 = 128 + ((-38 * r0 - 74 * g0 + 112 * b0 + 128) >> 8);
					int v0 = 128 + ((112 * r0 - 94 * g0 - 18 * b0 + 128) >> 8);
					int y1 = 16 + ((66 * r1 + 129 * g1 + 25 * b1 + 128) >> 8);
					int u1 = 128 + ((-38 * r1 - 74 * g1 + 112 * b1 + 128) >> 8);
					int v1 = 128 + ((112 * r1 - 94 * g1 - 18 * b1 + 128) >> 8);
					int y2 = 16 + ((66 * r2 + 129 * g2 + 25 * b2 + 128) >> 8);
					int u2 = 128 + ((-38 * r2 - 74 * g2 + 112 * b2 + 128) >> 8);
					int v2 = 128 + ((112 * r2 - 94 * g2 - 18 * b2 + 128) >> 8);
					int y3 = 16 + ((66 * r3 + 129 * g3 + 25 * b3 + 128) >> 8);
					int u3 = 128 + ((-38 * r3 - 74 * g3 + 112 * b3 + 128) >> 8);
					int v3 = 128 + ((112 * r3 - 94 * g3 - 18 * b3 + 128) >> 8);

					// pack to YUV420
					int u = (u0 + u1 + u2 + u3) / 4;
					int v = (v0 + v1 + v2 + v3) / 4;

					// and write block to destination
					dest[y * 2 * VIDEO_XSIZE + x * 2] = y0;
					dest[y * 2 * VIDEO_XSIZE + x * 2 + 1] = y1;
					dest[(y * 2 + 1) * VIDEO_XSIZE + x * 2] = y2;
					dest[(y * 2 + 1) * VIDEO_XSIZE + x * 2 + 1] = y3;
					dest[VIDEO_XSIZE * VIDEO_YSIZE + y * VIDEO_XSIZE / 2 + x] = u;
					dest[VIDEO_XSIZE * VIDEO_YSIZE + VIDEO_XSIZE * VIDEO_YSIZE / 4 + y * VIDEO_XSIZE / 2 + x] = v;
				}
			break;
	}
}


void Camera::Run()
{
	if(cfd != -1)
	{
		// enqueue buffers
		for(int i = 0; i < MAX_BUFFERS; i++)
		{
			v4l2_buffer buf;
			memset(&buf,0,sizeof(v4l2_buffer));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			if(ioctl(cfd,VIDIOC_QBUF,&buf) == -1)
			{
				printf("Camera::Run: cannot enqueue buffers\n");
				return;
			}
		}

		// start streaming
		int buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(ioctl(cfd,VIDIOC_STREAMON,&buf_type) == -1)
		{
			printf("Camera::Run: cannot start streaming\n");
			return;
		}
	}

	Task::Run();

	if(cfd != -1)
	{
		// stop streaming
		int buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(ioctl(cfd,VIDIOC_STREAMOFF,&buf_type) == -1)
		{
			printf("Camera::Run: cannot stop streaming\n");
			return;
		}
	}
}


void Camera::Step()
{
	// prepare poll structures
	struct pollfd pfd[2];

	// message pipe
	pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	// camera
	pfd[1].fd = cfd;
	pfd[1].events = POLLIN;
	pfd[1].revents = 0;

	// do the poll
	poll(pfd,2,-1);

	// check for incoming messages on the pipe
	if(pfd[0].revents & POLLIN)
		FlushMessages();

	// check for incoming video frames
	if(pfd[1].revents & POLLIN)
	{
		// dequeue buffer
		v4l2_buffer buf;
		memset(&buf,0,sizeof(v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if(ioctl(cfd,VIDIOC_DQBUF,&buf) == -1)
		{
			printf("Camera::Step: cannot dequeue buffer\n");
			return;
		}

		if(type != VIDEO_UNKNOWN)
		{
			// create new frame
			videobuffer* video = videopool.get();

			// convert from mapped buffer
			ConvertToYUV(video->data,buffer[buf.index].mapped,type);

			// send to server
			//printf("Camera: posting videobuffer to server\n");
			server.Post(ITM_VIDEO,video);
		}

		// enqueue buffer
		ioctl(cfd,VIDIOC_QBUF,&buf);
	}
}
