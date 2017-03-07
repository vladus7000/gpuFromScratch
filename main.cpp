#include <iostream>
#include "gpu/GraphicsAPI.h"
#include "BMPWriter.h"
#include "teapot.h"

#include <string>
#include <algorithm>

using namespace gapi;

int main()
{
	GraphicsAPI myApi(1700, 1700, GraphicsAPI::Y_AXIS_TOP);
	myApi.clear(Pixel(0.4f, 0.5f, 0.4f));

	//myApi.loadPicture("Preview.jpg");
	myApi.loadTexture("Preview.jpg");
	//myApi.setPixel(Point2(0, 0), Pixel(0.0f, 1.0f, 0.0f));
	//myApi.setPixel(Point2(199, 99), Pixel(0.0f, 0.0f, 1.0f));

	//myApi.drawLine(Point2(0, 50), Point2(15, 80), Pixel(0.0f, 0.0f, 1.0f));

//	myApi.drawCircle(Point2(100, 50), 50, Pixel(1.0f, 1.0f, 0.0f), !true);

	//myApi.drawText(Point2(80, 80), "Hello Vlad R.", Pixel(1.0f, 1.0f, 0.0f));

	/*for (unsigned int x = 0; x < myApi.getWidth(); x++)
	{
		for (unsigned int y = 0; y < myApi.getHeight(); y++)
		{
			myApi.setPixel(Point2(x, y), myApi.getPixel(Point2(x, y)));
		}
	}
	*/

	Point3* points = new Point3[TEAPOT_NUM_VERTS];
	Point3* norms = new Point3[TEAPOT_NUM_VERTS];
	Point3* tc = new Point3[TEAPOT_NUM_VERTS];

	unsigned int* indices = new unsigned int[TEAPOT_NUM_INDICES];

	for (int i = 0; i < TEAPOT_NUM_VERTS; i++)
	{
		points[i].x = teapot_verts[i][0];
		points[i].y = teapot_verts[i][1];
		points[i].z = teapot_verts[i][2];

		tc[i].x = teapot_verts[i][4];
		tc[i].y = teapot_verts[i][5];

		norms[i].x = teapot_verts[i][6 + 6]; //0, 3, 6
		norms[i].y = teapot_verts[i][7 + 6];
		norms[i].z = teapot_verts[i][8 + 6];
	}

	for (int i = 0; i < TEAPOT_NUM_INDICES; i++)
	{
		indices[i] = teapot_indices[i];
	}

	writer::BMPWriter writer;
	int nFrames = 1;
	for (int i = 0; i < nFrames; i++)
	{
		std::cout << "rendering " << i + 1 << " frame from " << nFrames << " ... \n";
		
		for (int t = 0; t < 6; t++)
		{
			myApi.clear(Pixel(0.4f, 0.5f, 0.4f));
			if (t == 0)
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y)) * std::max(0.0f, dot(p.data[1] * -1.0f, Point4(-0.5, -0.5, 0.5, 0.0f))) + Point4(0.08f, 0.08f, 0.08f, 0);
				};

				std::cout << "rendering teapot ... \n";
				myApi.setVertexBuffer(points, TEAPOT_NUM_VERTS);
				myApi.setIndexBuffer(indices, TEAPOT_NUM_INDICES);
				myApi.setNBuffer(norms, 0);
				myApi.setNBuffer(tc, 1);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);

				myApi.DrawIndexed(TEAPOT_NUM_INDICES, 0, 0);

			}
			else if (t == 1)
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y));
				};

				std::cout << "rendering triangle 1 ... \n";
				Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				Point3 tc[] = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				myApi.setVertexBuffer((Point3*)ps, 3);
				myApi.setNBuffer((Point3*)tc, 1);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);
				myApi.Draw(3, 0);
			}
			else if (t == 2)
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y));// *std::max(0.0f, dot(p.data[1] * -1.0f, Point4(-0.5, -0.5, 0.5, 0.0f))) + Point4(0.08f, 0.08f, 0.08f, 0);
				};

				std::cout << "rendering triangle 2 ... \n";
				Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ 0.0f, 0.375f, 0.0f },{ 1.0f, -1.0f, 0.0f } };
				Point3 tc[] = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				myApi.setVertexBuffer((Point3*)ps, 3);
				myApi.setNBuffer((Point3*)tc, 1);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);
				myApi.Draw(3, 0);
			}
			else if (t == 3) // 3 tri
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y));// *std::max(0.0f, dot(p.data[1] * -1.0f, Point4(-0.5, -0.5, 0.5, 0.0f))) + Point4(0.08f, 0.08f, 0.08f, 0);
				};

				std::cout << "rendering triangle 3 ... \n";
				Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f } }; //  1 2 3
				//Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f } }; // 1 3 2
				//Point3 ps[] = { { 1.0f, 1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f } }; // 2 1 3
				//Point3 ps[] = { { 1.0f, 1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f } }; // 2 3 1
				//Point3 ps[] = { { 1.0f, -1.0f, 0.0f }, { -1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f } }; // 3 1 2
				//Point3 ps[] = { { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f }}; // 3 2 1
				//{ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.375f, 0.0f }, { 1.0f, -1.0f, 0.0f },
				//{ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.375f, 0.0f }, { 1.0f, -1.0f, 0.0f }
				Point3 tc[] = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				myApi.setVertexBuffer((Point3*)ps, 3);
				myApi.setNBuffer((Point3*)tc, 1);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);
				myApi.Draw(3, 0);
			}
			else if (t == 4)
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y));// *std::max(0.0f, dot(p.data[1] * -1.0f, Point4(-0.5, -0.5, 0.5, 0.0f))) + Point4(0.08f, 0.08f, 0.08f, 0);
				};

				std::cout << "rendering triangle 4 ... \n";
				//Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f } }; //  1 2 3
				//Point3 ps[] = { { -1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ -1.0f, 1.0f, 0.0f } }; //  1 3 2
				//Point3 ps[] = { { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 0.0f } }; //  2 3 1
				//Point3 ps[] = { { -1.0f, 1.0f, 0.0f } ,{ -1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f } }; //  2 1 3
				//Point3 ps[] = { { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f } }; //  3 2 1
				Point3 ps[] = { { 1.0f, 1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f },{ -1.0f, 1.0f, 0.0f } }; //  3 1 2

				Point3 tc[] = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				myApi.setVertexBuffer((Point3*)ps, 3);
				myApi.setNBuffer((Point3*)tc, 1);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);
				myApi.Draw(3, 0);
			}
			else
			{
				auto v = [](ShaderIO& i)
				{
					Matrix4x4 m;
					m.setScale(0.5f, 0.5f, 0.5f);
					//i.data[0] = m * i.data[0];
				};

				auto p = [&](int x, int y, ShaderIO& p, PSOutput& out)
				{
					//Point4(0.5f, 0.5f, 0.0f, 0)
					out.colorBuffer = myApi.sampleTexture(Point2(p.data[2].x, p.data[2].y));// *std::max(0.0f, dot(p.data[1] * -1.0f, Point4(-0.5, -0.5, 0.5, 0.0f))) + Point4(0.08f, 0.08f, 0.08f, 0);
				};
				std::cout << "rendering full screen quad ... \n";
				Point3 ps[] = { { -1.0f + 1.0f/1000.0f, -1.0f + 1.0f / 1000.0f, 0.0f },{ -1.0f + 1.0f / 1000.0f, 1.0f - 1.0f / 1000.0f, 0.0f },{ 1.0f - 1.0f / 1000.0f, 1.0f - 1.0f / 1000.0f, 0.0f },{ 1.0f- 1.0f / 1000.0f, -1.0f+ 1.0f / 1000.0f, 0.0f } };
				Point3 tc[] = { { 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };
				unsigned int in[] = { 0,2,1, 0, 2, 3 };
				myApi.setVertexBuffer((Point3*)ps, 4);
				myApi.setNBuffer((Point3*)tc, 1);
				myApi.setIndexBuffer(in, 6);
				myApi.setVertexShader(v);
				myApi.setPixelShader(p);
				myApi.DrawIndexed(6, 0, 0);

			}
			myApi.resolveFB();
			auto frameBuffer = myApi.getFrameBuffer();
			std::string s("picture");
			s += std::to_string(i + 1);
			s += std::to_string(t);
			s += ".bmp";
			writer.writeRGB_Float(s.c_str(), frameBuffer, myApi.getWidth(), myApi.getHeight());
		}
	}

	delete[] points;
	delete[] indices;
	delete[] tc;
}