/* ======================================
 * CornerFix - Fixing the M8 Cyan Corners
 * ======================================
 *
 * Project Info:  http://sourceforge.net/projects/cornerfix
 * Project Lead:  Sandy McGuffog (sandy.cornerfix@gmail.com);
 *
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * ---------------
 * Form1.h
 * ---------------
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version;
 * 18 Jun 2007 : 0.9.0.5
 * 21 Jun 2007 : 0.9.0.6
 * 1 Jul 2007 : 0.9.1.0
 * 11 Sept 2007 : 0.9.1.3
 * 23 Oct 2007 : 0.9.1.5
 * 21 Dec 2007 : 0.9.1.8
 * 2 December 2008 : 1.0.0.0;
 * 12 June 2009 : 1.1.0.0;
 * 14 July 2008 : 1.1.0.1; fix bug that could could cause a "bad parameter"
 *                or "security" exception.
 * 6 October 2010 : 1.4.0.0
 * 4 April 2011 : 1.4.1.1
 * 31 July 2012 : 1.4.2.2
 *
 */

/*
 * The main app form for CornerFix. Why is all this in a .H file?
 * Ask the guys that built Visual Studio!
 */


#pragma once
#include "dng_win_glue.h"
#include "dng_errors.h"
#include "DisplayOptions.h"
#include "CreationOptions.h"
#include "ImageOptions.h"
#include "MakerNoteOptions.h"
#include "CornerFixRegistry.h"
#include "CornerFixVersion.h"
#include "tiffThumb.h"

//#define genCSVFile



namespace cornerfixw {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices; // for class Marshal
	using namespace System::Diagnostics;



	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{

	private:
		DisplayOptions^ displayChild;
		CreationOptions^ creationChild;
		ImageOptions^ imageChild;
		MakerNoteOptions^ makerNoteChild;
		CornerFixRegistry^ registry;
		String^ tempFile;
		array<String^>^ loadedCPFFiles;
		array<String^>^ imageFiles;
		array<String^>^ cpfFiles;
		array<String^>^ outputFiles;
		String^ readyMsg;
		int progressState;
		int validateRetVal;
		bool batchMode;

	private:
		
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::ToolStripMenuItem^  optionsToolStripMenuItem;


	private: System::Windows::Forms::ToolStripMenuItem^  batchProcessImagesToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  batchCreateToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  displayOptionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  imageCorrectionOptionsToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^  cpfOpenFileDialog;
	private: System::Windows::Forms::SaveFileDialog^  cpfSaveFileDialog;
	private: System::Windows::Forms::ToolStripMenuItem^  makerNoteOptionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cornerFixHelpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  profileCreationToolStripMenuItem;



	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			displayChild = gcnew DisplayOptions();
			creationChild = gcnew CreationOptions();
			imageChild = gcnew ImageOptions();
			makerNoteChild = gcnew MakerNoteOptions();
			imageChild->imageApply += gcnew System::EventHandler(this, &Form1::imageApplyHandler);
			glue = new dng_win_glue();
			dng_local_printf::setBox(richTextBox1);
			dng_local_printf::setWorker(this->backgroundWorker1);
			tempFile = "";
			loadedCPFFiles = nullptr;
			readyMsg = "Ready......";
			progressState = -1;
			validateRetVal = 0;
			batchMode = false;
			registry = gcnew CornerFixRegistry();
			if (registry->get("imageDirectory", "") != "")
			{
				this->openFileDialog1->InitialDirectory = registry->get("imageDirectory", "");
			}
			if (registry->get("cpfDirectory", "") != "")
			{
				this->cpfOpenFileDialog->InitialDirectory = registry->get("cpfDirectory", "");
			}

			try 
			{
			  // Create a 0-length temporary file					
				tempFile = Path::GetTempFileName();
				// Set the file's attributes to "Temporary" for
				// better performance
				FileInfo^ myFileInfo = gcnew FileInfo(tempFile);
				myFileInfo->Attributes = FileAttributes::Temporary;
				glue->setOutputFile(tempFile);
			} 
			catch(Exception^ ex)
			{
			  // handle exceptions
				MessageBox::Show(ex->Message+"Could not create temporary file - could be a permissions problem. Exiting",
				 "Cornerfix for Windows", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			}
//			System::Resources::ResourceManager^ rc = System::Resources::ResourceManager();
//			System::Windows::F
//			System::Resources::ResourceManager::GetObject("app.ico");
//			System::Drawing::Icon^ icon1 = gcnew System::Drawing::Icon("app.ico");
//			this->Icon = icon1;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			registry->set("imageDirectory", openFileDialog1->InitialDirectory);
			registry->set("cpfDirectory", cpfOpenFileDialog->InitialDirectory);
			if (components)
			{
				delete components;
			}
			if (glue)
			{
				delete glue;
			}
			// Delete the temporary file..........
			if (File::Exists(tempFile)) {
				File::Delete(tempFile);
			}
			if (displayChild) {
				displayChild->Close();
			}
			if (creationChild) {
				creationChild->Close();
			}
			if (imageChild) {
				imageChild->Close();
			}
			if (makerNoteChild) {
				makerNoteChild->Close();
			}
		}
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openImageToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveImageToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  profileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  createToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  savenAsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::RichTextBox^  richTextBox1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

System::ComponentModel::Container ^components;

	private: System::Windows::Forms::ToolStripProgressBar^  toolStripProgressBar1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
			 dng_win_glue* glue;
		

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripProgressBar1 = (gcnew System::Windows::Forms::ToolStripProgressBar());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openImageToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveImageToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->batchProcessImagesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->profileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->createToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->savenAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->batchCreateToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->displayOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->profileCreationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->imageCorrectionOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->makerNoteOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->cpfOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->cpfSaveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->cornerFixHelpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->toolStripStatusLabel1, 
				this->toolStripProgressBar1});
			this->statusStrip1->LayoutStyle = System::Windows::Forms::ToolStripLayoutStyle::HorizontalStackWithOverflow;
			this->statusStrip1->Location = System::Drawing::Point(0, 463);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(633, 22);
			this->statusStrip1->TabIndex = 0;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(54, 17);
			this->toolStripStatusLabel1->Text = L"Ready.....";
			this->toolStripStatusLabel1->ToolTipText = L"Tells you what CornerFix is doing";
			// 
			// toolStripProgressBar1
			// 
			this->toolStripProgressBar1->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
			this->toolStripProgressBar1->Name = L"toolStripProgressBar1";
			this->toolStripProgressBar1->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->toolStripProgressBar1->Size = System::Drawing::Size(300, 16);
			this->toolStripProgressBar1->ToolTipText = L"Gives you an indication of how far into its prcessing cycle CornerFix is";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->fileToolStripMenuItem, 
				this->profileToolStripMenuItem, this->optionsToolStripMenuItem, this->helpToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(633, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->openImageToolStripMenuItem, 
				this->saveImageToolStripMenuItem, this->batchProcessImagesToolStripMenuItem, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// openImageToolStripMenuItem
			// 
			this->openImageToolStripMenuItem->Name = L"openImageToolStripMenuItem";
			this->openImageToolStripMenuItem->Size = System::Drawing::Size(187, 22);
			this->openImageToolStripMenuItem->Text = L"Open Image";
			this->openImageToolStripMenuItem->ToolTipText = L"Open a DNG image";
			this->openImageToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openImageToolStripMenuItem_Click);
			// 
			// saveImageToolStripMenuItem
			// 
			this->saveImageToolStripMenuItem->Name = L"saveImageToolStripMenuItem";
			this->saveImageToolStripMenuItem->Size = System::Drawing::Size(187, 22);
			this->saveImageToolStripMenuItem->Text = L"Save Image";
			this->saveImageToolStripMenuItem->ToolTipText = L"Save the processed image to a DNG file";
			this->saveImageToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveImageToolStripMenuItem_Click);
			// 
			// batchProcessImagesToolStripMenuItem
			// 
			this->batchProcessImagesToolStripMenuItem->Name = L"batchProcessImagesToolStripMenuItem";
			this->batchProcessImagesToolStripMenuItem->Size = System::Drawing::Size(187, 22);
			this->batchProcessImagesToolStripMenuItem->Text = L"Batch Correct Images";
			this->batchProcessImagesToolStripMenuItem->ToolTipText = L"Allows multiple files to be selected for correction in batch mode using the curre" 
				L"ntly loaded profile; Files are renamed with _CF";
			this->batchProcessImagesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::batchProcessImagesToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(187, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->ToolTipText = L"Exit from CornerFix for Windows";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// profileToolStripMenuItem
			// 
			this->profileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->loadToolStripMenuItem, 
				this->createToolStripMenuItem, this->savenAsToolStripMenuItem, this->batchCreateToolStripMenuItem});
			this->profileToolStripMenuItem->Name = L"profileToolStripMenuItem";
			this->profileToolStripMenuItem->Size = System::Drawing::Size(80, 20);
			this->profileToolStripMenuItem->Text = L"Lens Profile";
			// 
			// loadToolStripMenuItem
			// 
			this->loadToolStripMenuItem->Name = L"loadToolStripMenuItem";
			this->loadToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->loadToolStripMenuItem->Text = L"Open";
			this->loadToolStripMenuItem->ToolTipText = L"Open an existing lens profile (.CPF) file";
			this->loadToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::loadToolStripMenuItem_Click);
			// 
			// createToolStripMenuItem
			// 
			this->createToolStripMenuItem->Name = L"createToolStripMenuItem";
			this->createToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->createToolStripMenuItem->Text = L"Create";
			this->createToolStripMenuItem->ToolTipText = L"Create a lens profile from the loaded reference image";
			this->createToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::createToolStripMenuItem_Click);
			// 
			// savenAsToolStripMenuItem
			// 
			this->savenAsToolStripMenuItem->Name = L"savenAsToolStripMenuItem";
			this->savenAsToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->savenAsToolStripMenuItem->Text = L"Save as";
			this->savenAsToolStripMenuItem->ToolTipText = L"Save the lens profile to a file";
			this->savenAsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::savenAsToolStripMenuItem_Click);
			// 
			// batchCreateToolStripMenuItem
			// 
			this->batchCreateToolStripMenuItem->Name = L"batchCreateToolStripMenuItem";
			this->batchCreateToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->batchCreateToolStripMenuItem->Text = L"Batch Create";
			this->batchCreateToolStripMenuItem->ToolTipText = L"Allows multiple files to be selectd for batch mode conversion to profiles; Files " 
				L"have the same name as the image, but the extension CPF";
			this->batchCreateToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::batchCreateToolStripMenuItem_Click);
			// 
			// optionsToolStripMenuItem
			// 
			this->optionsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->displayOptionsToolStripMenuItem, 
				this->profileCreationToolStripMenuItem, this->imageCorrectionOptionsToolStripMenuItem, this->makerNoteOptionsToolStripMenuItem});
			this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
			this->optionsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->optionsToolStripMenuItem->Text = L"Options";
			// 
			// displayOptionsToolStripMenuItem
			// 
			this->displayOptionsToolStripMenuItem->Name = L"displayOptionsToolStripMenuItem";
			this->displayOptionsToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->displayOptionsToolStripMenuItem->Text = L"Display Options....";
			this->displayOptionsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::displayOptionsToolStripMenuItem_Click);
			// 
			// profileCreationToolStripMenuItem
			// 
			this->profileCreationToolStripMenuItem->Name = L"profileCreationToolStripMenuItem";
			this->profileCreationToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->profileCreationToolStripMenuItem->Text = L"Profile Creation Options....";
			this->profileCreationToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::profileCreationToolStripMenuItem_Click);
			// 
			// imageCorrectionOptionsToolStripMenuItem
			// 
			this->imageCorrectionOptionsToolStripMenuItem->Name = L"imageCorrectionOptionsToolStripMenuItem";
			this->imageCorrectionOptionsToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->imageCorrectionOptionsToolStripMenuItem->Text = L"Image Correction Options....";
			this->imageCorrectionOptionsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::imageCorrectionOptionsToolStripMenuItem_Click);
			// 
			// makerNoteOptionsToolStripMenuItem
			// 
			this->makerNoteOptionsToolStripMenuItem->Name = L"makerNoteOptionsToolStripMenuItem";
			this->makerNoteOptionsToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->makerNoteOptionsToolStripMenuItem->Text = L"MakerNote Options....";
			this->makerNoteOptionsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::makerNoteOptionsToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->aboutToolStripMenuItem, 
				this->cornerFixHelpToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->aboutToolStripMenuItem->Text = L"About";
			this->aboutToolStripMenuItem->ToolTipText = L"Application Information";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 24);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->richTextBox1);
			this->splitContainer1->Size = System::Drawing::Size(633, 439);
			this->splitContainer1->SplitterDistance = 336;
			this->splitContainer1->TabIndex = 3;
			// 
			// splitContainer2
			// 
			this->splitContainer2->BackColor = System::Drawing::SystemColors::Control;
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->ForeColor = System::Drawing::SystemColors::Control;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->pictureBox1);
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->pictureBox2);
			this->splitContainer2->Size = System::Drawing::Size(633, 336);
			this->splitContainer2->SplitterDistance = 315;
			this->splitContainer2->SplitterWidth = 3;
			this->splitContainer2->TabIndex = 0;
			// 
			// pictureBox1
			// 
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(315, 336);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// pictureBox2
			// 
			this->pictureBox2->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBox2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox2->Location = System::Drawing::Point(0, 0);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(315, 336);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox2->TabIndex = 0;
			this->pictureBox2->TabStop = false;
			// 
			// richTextBox1
			// 
			this->richTextBox1->DetectUrls = false;
			this->richTextBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->richTextBox1->Location = System::Drawing::Point(0, 0);
			this->richTextBox1->MaximumSize = System::Drawing::Size(10000, 10000);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->ReadOnly = true;
			this->richTextBox1->Size = System::Drawing::Size(633, 99);
			this->richTextBox1->TabIndex = 0;
			this->richTextBox1->Text = L"";
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->WorkerReportsProgress = true;
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork);
			this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorker1_RunWorkerCompleted);
			this->backgroundWorker1->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::backgroundWorker1_ProgressChanged);
			// 
			// cornerFixHelpToolStripMenuItem
			// 
			this->cornerFixHelpToolStripMenuItem->Name = L"cornerFixHelpToolStripMenuItem";
			this->cornerFixHelpToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->cornerFixHelpToolStripMenuItem->Text = L"CornerFix Help";
			this->cornerFixHelpToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cornerFixHelpToolStripMenuItem_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(633, 485);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->menuStrip1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"CornerFix for Windows";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion




/// <summary>
/// Gets the thumbnail from the image metadata. Returns null of no thumbnail
/// is stored in the image metadata
/// </summary>
/// <param name="path"></param>
/// <returns></returns>

private: Image ^ GetThumbnail (String ^ path)
{
	tiffThumb *decoder = nullptr;
	try {
		decoder = new tiffThumb();
		bool haveThumbnail;

		// This is total insanity; just crazy..........
		const char* str2 = (const char*)(Marshal::StringToHGlobalAnsi(path)).ToPointer();
		haveThumbnail = decoder->parseFile(str2);
		Marshal::FreeHGlobal((System::IntPtr)(void*)str2);

		// The image data is in the form of a byte array. First convert
		// to a managed array, then use the managed array to write all 
		// the bytes to a stream and create a new image from that stream
		if (haveThumbnail) {
			size_t size;
			uint8_t *buffer;
			decoder->getBuffer(&size, &buffer);
			array<System::Byte>^ imageBytes = gcnew array<System::Byte>(size);

			Marshal::Copy((IntPtr)buffer, imageBytes, 0, size);
			delete decoder;
			decoder = nullptr;
			MemoryStream ^ stream = gcnew MemoryStream (imageBytes->Length);
			if (stream == nullptr) {
				return nullptr;
			}
			else {
				stream->Write (imageBytes, 0, imageBytes->Length);
				return Image::FromStream(stream, false, false);
			}
		}
		else {
			delete decoder;
			return nullptr;
		}
	}
	catch (...) {
		if (decoder != nullptr) {
			delete decoder;
		}
		return nullptr;
	}
}


private:		array<String^>^ convertExtensions(array<String^>^ files, String ^ extension) {
				
				array<String^>^ retVal; 

				if (files != nullptr) 
				{
					retVal = gcnew array<String^>(files->Length);
					int index  = 0;
					for each ( String^ file in files )
					{
						retVal->SetValue(Path::ChangeExtension(file, extension), index++);
					}
				}
				else {
					retVal = nullptr;
				}

				return retVal;

				}

private:		array<String^>^ renameFiles(array<String^>^ files, String ^ suffix) {
				
				array<String^>^ retVal; 

				if (files != nullptr) 
				{
					retVal = gcnew array<String^>(files->Length);
					int index  = 0;
					for each ( String^ file in files )
					{
						retVal->SetValue(Path::ChangeExtension( file, nullptr )+suffix+
							Path::GetExtension(file), index++);
					}
				}
				else {
					retVal = nullptr;
				}

				return retVal;

				}


private:		void updateDisplay() {

					if ((imageFiles) && (imageFiles->Length > 0 ))
					{
						this->Text = Path::GetFileNameWithoutExtension((String ^) imageFiles->GetValue(0))+
							" - CornerFix for Windows";
						System::Drawing::Image ^thumb = GetThumbnail ((String ^) imageFiles->GetValue(0));
						if (thumb == nullptr) {
							// Try an see - nothing to lose at this point....
							this->pictureBox1->LoadAsync((String ^) imageFiles->GetValue(0));
						}
						else {
							// We have a TIFF thumnail
							this->pictureBox1->Image = thumb;
						}
					}
					else {
						this->Text = "CornerFix for Windows";
					}
					switch (glue->getCPFState()) {
						case kcpfStateNone:
							readyMsg = "Ready....";
							break;
						case kcpfStateCreated:
							readyMsg = "Profile: <"+Path::GetFileNameWithoutExtension((String ^) cpfFiles->GetValue(0))+">....";
							break;
						case kcpfStateSaved:
							readyMsg = "Profile: "+Path::GetFileNameWithoutExtension((String ^) cpfFiles->GetValue(0))+"....";
							break;
						default:
							readyMsg = "Ready....";
							break;
					}
					this->toolStripStatusLabel1->Text = readyMsg;



				}

private: void imageApplyHandler(System::Object^ sender, System::EventArgs^  e) 
	{
		 processImages();
	}


private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
		this->toolStripStatusLabel1->Text = "Ready....."; 
		this->richTextBox1->AppendText("CornerFix for Windows Version "+CornerFixVersion+"\nCopyright © 2007-2011 Sandy McGuffog\n" +
						"Portions Copyright © 2006 Adobe Systems Incorporated\nAll Rights Reserved.\n"+
						"Lossless JPEG code adapted from:\n" +
						"Copyright (C) 1991, 1992, Thomas G. Lane.\n" +
						"Part of the Independent JPEG Group's software.\n" +
						"See the file Copyright for more details.\n" +
						"Copyright (c) 1993 Brian C. Smith,\n" +
						"The Regents of the University of California\n" +
						"All rights reserved.\n" +
						"Copyright (c) 1994 Kongji Huang and Brian C. Smith.\n" +
						"Cornell University\n" +
						"All rights reserved.\n" +
						"Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without written agreement is hereby granted, provided that the above copyright notice and the following two paragraphs appear in all copies of this software.\n" +
						"IN NO EVENT SHALL CORNELL UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF CORNELL UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. \n" +
						"CORNELL UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN ""AS IS"" BASIS, AND CORNELL UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\n" +
						"CornerFix Icon adapted from Carlitus (Carles Carbonell Bernado)'s KDE icon set\n\n" +

						"This program is free software; you can redistribute it and/or\n"+
						"modify it under the terms of the GNU General Public License\n"+
						"as published by the Free Software Foundation; either version 2\n"+
						"of the License, or (at your option) any later version.\n"+
						"\n"+
						"This program is distributed in the hope that it will be useful,\n"+
						"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"+
						"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"+
						"GNU General Public License for more details.\n"+
						"\n"+
						"You should have received a copy of the GNU General Public License\n"+
						"along with this program; if not, write to the Free Software\n"+
						"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n"
						);
		 }


private: System::Void openImageToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 Stream^ myStream;
			 this->openFileDialog1->Filter = "dng files (*.dng)|*.dng|All files (*.*)|*.*";
			 this->openFileDialog1->FilterIndex = 1;

			 if ( this->openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
			 {
				 if ( (myStream = openFileDialog1->OpenFile()) != nullptr )
				 {
					 myStream->Close();
					FileInfo^ myFileInfo = gcnew FileInfo(openFileDialog1->FileName);
					if ((myFileInfo->Exists) && (myFileInfo->Length > 10000))
					{
						imageFiles = openFileDialog1->FileNames;
					}
					 this->openFileDialog1->InitialDirectory = Path::GetDirectoryName(openFileDialog1->FileName);
					 if (this->openFileDialog1->InitialDirectory == "") {
						this->saveFileDialog1->InitialDirectory = this->openFileDialog1->InitialDirectory;
						}
					glue->setImage(openFileDialog1->FileName);
					updateDisplay();
					processImages();

				 }
			 }
		 }

private: System::Void savenAsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				this->cpfSaveFileDialog->Filter = "Lens profile files (*.cpf)|*.cpf|All files (*.*)|*.*";
				this->cpfSaveFileDialog->FilterIndex = 1;
				this->cpfSaveFileDialog->AddExtension = true;
				this->cpfSaveFileDialog->DefaultExt = ".cpf";
				if (glue->getCPFState() != kcpfStateNone) 
				{
   					this->cpfSaveFileDialog->FileName = (String ^) cpfFiles->GetValue(0);
					 if ( this->cpfSaveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
					 {
						try
						{
							if (glue->saveCPFFile(cpfSaveFileDialog->FileName)) {
								cpfFiles = this->cpfSaveFileDialog->FileNames;
							}
							else {
							MessageBox::Show("File Write Error",
							 "Cornerfix for Windows", MessageBoxButtons::OK,
							 MessageBoxIcon::Error);
							}
						}
						catch(Exception^ ex)
						{
						  // handle exceptions
							MessageBox::Show("File Write Error " + ex->Message,
							 "Cornerfix for Windows", MessageBoxButtons::OK,
							 MessageBoxIcon::Error);
						}
						updateDisplay();
					    this->cpfSaveFileDialog->InitialDirectory = Path::GetDirectoryName(cpfSaveFileDialog->FileName);
					 }
				}
				else 
				 {
					MessageBox::Show("No profile has been loaded or created",
					 "Cornerfix for Windows", MessageBoxButtons::OK,
					 MessageBoxIcon::Error);
				 }
		 }


private: System::Void saveImageToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				this->saveFileDialog1->Filter = "dng files (*.dng)|*.dng|All files (*.*)|*.*";
				this->saveFileDialog1->FilterIndex = 1;
				this->saveFileDialog1->DefaultExt = ".dng";
				if (imageFiles && (imageFiles->Length > 0))
				 {
					 saveFileDialog1->FileName =  Path::ChangeExtension( (String ^) imageFiles->GetValue(0), nullptr )+"_CF";

					 if ( this->saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
					 {
						try
						{
							File::Copy(tempFile, saveFileDialog1->FileName, true);
							imageFiles = openFileDialog1->FileNames;
						}
						catch(Exception^ ex)
						{
						  // handle exceptions
							MessageBox::Show("File Write Error " + ex->Message,
							 "Cornerfix for Windows", MessageBoxButtons::OK,
							 MessageBoxIcon::Error);
						}
					    this->saveFileDialog1->InitialDirectory = Path::GetDirectoryName(saveFileDialog1->FileName);
					 }
				 }
				 else
				 {
					MessageBox::Show("No file is loaded",
					 "Cornerfix for Windows", MessageBoxButtons::OK,
					 MessageBoxIcon::Error);

				 }
		 }


private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 Application::Exit();
		 }


private: System::Void loadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 Stream^ myStream;
				 this->cpfOpenFileDialog->Filter = "Lens profile files (*.cpf)|*.cpf|All files (*.*)|*.*";
				 this->cpfOpenFileDialog->FilterIndex = 1;
				 this->cpfOpenFileDialog->FileName = "";

				 if ( this->cpfOpenFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
				 {
					 if ( (myStream = cpfOpenFileDialog->OpenFile()) != nullptr )
					 {
						 myStream->Close();
						 if (!glue->setCPFFile(cpfOpenFileDialog->FileName)) 
						 {
							MessageBox::Show("That does not appear to be a valid CPF file ",
							 "Cornerfix for Windows", MessageBoxButtons::OK,
							 MessageBoxIcon::Error);
						 }
						 cpfFiles = cpfOpenFileDialog->FileNames;
	    				 updateDisplay();
						 processImages();
					 }
					 this->cpfOpenFileDialog->InitialDirectory = Path::GetDirectoryName(cpfOpenFileDialog->FileName);
					 if (this->cpfSaveFileDialog->InitialDirectory == "") {
						this->cpfSaveFileDialog->InitialDirectory = this->cpfOpenFileDialog->InitialDirectory;
						}
				 }
		 }

private: System::Void createToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (imageFiles) 
			 {
				glue->setBuildCPF();
				processImages();
				cpfFiles = convertExtensions(imageFiles, ".cpf" );
				updateDisplay();
			 }
			 else 
			 {
				MessageBox::Show("No reference image file loaded",
				 "Cornerfix for Windows", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);

			 }
		 }

private: System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			MessageBox::Show("CornerFix for Windows Version "+CornerFixVersion+" \nCopyright © 2007-2013 Sandy McGuffog\n" +
						"Portions Copyright © 2006 Adobe Systems Incorporated\n"+
						"Portions Copyright © 1991, 1992, Thomas G. Lane.\n" +
						"Portions Copyright © 1993 Brian C. Smith, The Regents of the University of California\n" +
						"Portions Copyright © 1994 Kongji Huang and Brian C. Smith, Cornell University\n" +
						"All Rights Reserved.\n" +
						"Licenced under the GPL - see http://www.gnu.org/copyleft/gpl.html",
						 "Cornerfix for Windows", MessageBoxButtons::OK,
						 MessageBoxIcon::None);
		 }


private: void processImages(void) {

		 this->Cursor = Cursors::WaitCursor;
		 this->menuStrip1->Enabled = false;
		 this->toolStripProgressBar1->Enabled = true;
 		 this->toolStripStatusLabel1->Text = "Processing.....";
		 // Get rid of the current image - it's the old one....
		 this->pictureBox2->Image = nullptr;
#ifdef genCSVFile
#pragma message("WARNING - Compiling CSV Options in")
		 glue->setCSVFile(Path::ChangeExtension( (String ^) imageFiles->GetValue(0), ".csv" ));
		 glue->setTIFFile(Path::ChangeExtension( (String ^) imageFiles->GetValue(0), ".tif" ));
#endif
		 glue->setVerbose(displayChild->GetVerbose());
		 glue->setMultiple(creationChild->GetMultiple());
		 glue->setAsymetry(creationChild->GetBidirectional() ? 1.0 : 500.0);
		 glue->setDisableIlluminant(!creationChild->GetIlluminant());
		 glue->SetAntiAlias(imageChild->getAlias());
		 glue->SetLumaStrength(imageChild->getLuma());
		 glue->SetChromaStrength(imageChild->getChroma());
		 glue->setCompress(imageChild->getCompress());
		 glue->setLossless(imageChild->getLossless());
		 glue->setExifUpdate(makerNoteChild->GetExifUpdate());
		 glue->setCalculateAperture(makerNoteChild->GetCalculatedAperture());
		 glue->setClipApertures(makerNoteChild->GetClipApertures());
		 glue->setMaximizeResolution(imageChild->getMaximizeResolution());
		 glue->setOverwriteCameraName(imageChild->getAppleDNG());

		 // Start the asynchronous operation.
		 backgroundWorker1->RunWorkerAsync();
		 }

private: System::Void backgroundWorker1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				validateRetVal = dng_error_none;
				if (imageFiles != nullptr) 
				{
					int index = 0;
					for each ( String^ file in imageFiles )
					{
						if (validateRetVal == dng_error_none)
						{
							FileInfo^ myFileInfo = gcnew FileInfo(file);
							if ((myFileInfo->Exists) && (myFileInfo->Length > 10000))
							{
								if ((outputFiles) && (outputFiles->Length > 0))
								{
									glue->setOutputFile((String ^) outputFiles->GetValue(index));
								}
								else {
									glue->setOutputFile(tempFile);
									if ((cpfFiles) && (cpfFiles->Length > 0) && batchMode) 
									{
										glue->setBuildCPF();
										glue->setOutputFile("");
									}
									else 
									{
										glue->setOutputFile(tempFile);
									}

								}
								glue->setImage(file);
								validateRetVal = glue->dng_validate();
								if (!outputFiles && batchMode && (validateRetVal == dng_error_none))
								{
									 glue->saveCPFFile((String ^) cpfFiles->GetValue(index));;
								}

							}
						}
						index++;

					}
				}
		 }


private: System::Void backgroundWorker1_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) {
			 // This is a bit of a hack, but avoids passing untyped objects around.....
			 if (progressState != e->ProgressPercentage)
			 {
				progressState = e->ProgressPercentage;
				 switch (e->ProgressPercentage) {
					 case progressNone:
						 this->toolStripStatusLabel1->Text = "Ready.....";
						 this->toolStripProgressBar1->Value = 0;
						 break;
					 case progressReading:
						 this->toolStripStatusLabel1->Text = "Reading DNG.....";
						 this->toolStripProgressBar1->Value = 5;
						 break;
					 case progressDevignette:
						 this->toolStripStatusLabel1->Text = "Devignetting.....";
						 this->toolStripProgressBar1->Value = 10;
						 break;
					 case progressLinearization:
						 this->toolStripStatusLabel1->Text = "Linearizing.....";
						 this->toolStripProgressBar1->Value = 25;
						 break;
					 case progressIlluminant:
						 this->toolStripStatusLabel1->Text = "Illuminant compensation.....";
						 this->toolStripProgressBar1->Value = 45;
						 break;
					 case progressAnalysis:
						 this->toolStripStatusLabel1->Text = "Vignetting analysis.....";
						 this->toolStripProgressBar1->Value = 70;
						 break;
					 case progressInterpolate:
						 this->toolStripStatusLabel1->Text = "Interpolation.....";
						 this->toolStripProgressBar1->Value = 80;
						 break;
					 case progressRender:
						 this->toolStripStatusLabel1->Text = "Rendering.....";
						 this->toolStripProgressBar1->Value = 90;
						 break;
					 case progressWriteTIFF:
						 this->toolStripStatusLabel1->Text = "Writing TIFF.....";
						 this->toolStripProgressBar1->Value = 93;
						 break;
					 case progressThumbnail:
						 this->toolStripStatusLabel1->Text = "Building thumbnail.....";
						 this->toolStripProgressBar1->Value = 95;
						 break;
					 case progressWriteDNG:
						 this->toolStripStatusLabel1->Text = "Wring DNG.....";
						 this->toolStripProgressBar1->Value = 97;
						 break;
					 default:
						 this->toolStripStatusLabel1->Text = "Ready.....";
						 this->toolStripProgressBar1->Value = 0;
						 break;
				 }
			 }
		 
		 }

private: System::Void backgroundWorker1_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
     // First, handle the case where an exception was thrown.
      if ( e->Error != nullptr )
      {
		  MessageBox::Show( "Exception while processing: "+e->Error->Message, "Cornerfix for Windows" );
      }
	  else if ((validateRetVal != dng_error_none) && (validateRetVal != CornerFix_unknown_model) && (validateRetVal != CornerFix_saturated) && 
		  		 	(validateRetVal != CornerFix_bad_Bayer) && (validateRetVal != CornerFix_bad_linear) && (validateRetVal != CornerFix_bad_pixel_format)){

		  String ^s;

		  switch (validateRetVal) {

			case dng_error_unknown:       			
				s = "Logic or program error or other unclassifiable error.";
					break;
			case dng_error_not_yet_implemented:				
				s = "Functionality requested is not yet implemented.";
					break;
			case dng_error_silent:							
				s = "An error which should not be signalled to user.";
					break;
			case dng_error_user_canceled:					
				s = "Processing stopped by user (or host application) request.";
					break;
			case dng_error_host_insufficient:				
				s = "Necessary host functionality is not present.";
					break;
			case dng_error_memory:							
				s = "Out of memory.";
					break;
			case dng_error_bad_format:						
				s = "File format is not valid.";
					break;
			case dng_error_matrix_math:						
				s = "Matrix has wrong shape, is badly conditioned: or similar problem.";
					break;
			case dng_error_open_file:						
				s = "Could not open file.";
					break;
			case dng_error_read_file:						
				s = "Error reading file.";
					break;
			case dng_error_write_file:						
				s = "Error writing file.";
					break;
			case dng_error_end_of_file:					
				s = "Unexpected end of file.";
					break;
			default: s = "";

		  }

		  MessageBox::Show("DNG processing error: "+s,
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Error);

	  }
      else
	  {
		if (validateRetVal == CornerFix_unknown_model)
		{
		  MessageBox::Show("Warning - Unsupported DNG format\n",
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Warning);
		}
		else if (validateRetVal == CornerFix_saturated)
		{
		  MessageBox::Show("Warning - Reference image contains saturated highlights\n" +
			  "Your profile may not be accurate",
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Warning);
		}
		else if (validateRetVal == CornerFix_bad_Bayer)
		{
		  MessageBox::Show("Warning - CornerFix doesn't recognize the Bayer pattern\n" +
			  "You should try changing your DNG converter settings to Linear(demosaic) - see the CornerFix website for more information",
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Warning);
		}
		else if (validateRetVal == CornerFix_bad_linear)
		{
		  MessageBox::Show("Warning - CornerFix doesn't recognize the linear data format\n" +
			  "You should try changing your DNG converter settings to Linear(demosaic) - see the CornerFix website for more information",
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Warning);
		}
		else if (validateRetVal ==  CornerFix_bad_pixel_format)
		{
		  MessageBox::Show("Warning - CornerFix doesn't recognize the pixel format7\n" +
			  "You should try changing your DNG converter settings to Linear(demosaic) - see the CornerFix website for more information",
			 "Cornerfix for Windows", MessageBoxButtons::OK,
			 MessageBoxIcon::Warning);
		}

		if (!batchMode && imageFiles) {
			FileInfo^ myFileInfo = gcnew FileInfo(tempFile);
			if ((myFileInfo->Exists) && (myFileInfo->Length > 10000))
			{
				this->pictureBox2->Enabled = true;
				System::Drawing::Image ^thumb = GetThumbnail (tempFile);
				if (thumb == nullptr) {
					// Try an see - nothing to lose at this point....
					this->pictureBox2->LoadAsync(tempFile);
				}
				else {
					// We have a TIFF thumnail
					this->pictureBox2->Image = thumb;
				}
			}
		}
	  }
	
		if (batchMode) {
			imageFiles = nullptr;
			outputFiles = nullptr;
			batchMode = false;
			if (cpfFiles && cpfFiles->Length > 0) {
				cpfFiles = nullptr;
				glue->resetCPF();
			}
			else
			{
				cpfFiles = loadedCPFFiles;
			}

		}
		updateDisplay();
		this->Cursor = Cursors::Default;
		this->menuStrip1->Enabled = true;
		this->toolStripProgressBar1->Value = 0;
		this->imageChild->processingDone();
		progressState = -1;
	}


private: System::Void batchProcessImagesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->openFileDialog1->Filter = "dng files (*.dng)|*.dng|All files (*.*)|*.*";
			 this->openFileDialog1->FilterIndex = 1;
			 this->openFileDialog1->RestoreDirectory = true;
			 this->openFileDialog1->Multiselect = true;

			 if ( this->openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
			 {
				this->pictureBox1->Image = nullptr;
				this->pictureBox2->Image = nullptr;
				imageFiles = openFileDialog1->FileNames;
				outputFiles = renameFiles(imageFiles, "_CF" );
				loadedCPFFiles = cpfFiles;
				cpfFiles = nullptr;
				batchMode = true;
				processImages();
			 }

		 }


private: System::Void batchCreateToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->openFileDialog1->Filter = "dng files (*.dng)|*.dng|All files (*.*)|*.*";
			 this->openFileDialog1->FilterIndex = 1;
			 this->openFileDialog1->RestoreDirectory = true;
			 this->openFileDialog1->Multiselect = true;

			 if ( this->openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
			 {
				this->pictureBox1->Image = nullptr;
				this->pictureBox2->Image = nullptr;
				imageFiles = openFileDialog1->FileNames;
				cpfFiles = convertExtensions(imageFiles, ".cpf" );
				outputFiles = nullptr;
				batchMode = true;
				processImages();
			 }
		 }

private: System::Void displayOptionsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			displayChild->ShowDialog();
		 }
private: System::Void profileCreationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 creationChild->ShowDialog();
		 }
private: System::Void imageCorrectionOptionsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
   			 if (imageChild->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				this->processImages();
			 }

		 }
private: System::Void makerNoteOptionsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
   			 if (makerNoteChild->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				this->processImages();
			 }
		 }
private: System::Void cornerFixHelpToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			     Process::Start( "http://sites.google.com/site/cornerfix/using-cornerfix" );
		 }
};

};


		 

