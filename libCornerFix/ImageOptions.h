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
 * 18 Jun 2007 : 0.9.0.5
 * 21 Jun 2007 : 0.9.0.6
 * 21 Dec 2007 : 0.9.1.8
 *
 */

/*
 * Inage correction options for CornerFix
 */

#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CornerFixRegistry.h"

namespace cornerfixw {

	/// <summary>
	/// Summary for ImageOptions
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ImageOptions : public System::Windows::Forms::Form
	{
	public:
		ImageOptions(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			lumaTrackBar->Value = registry->get("lumaStrength", 100);
			chromaTrackBar->Value = registry->get("chromaStrength", 100);
			aliasTrackBar->Value = registry->get("antiAliasStrength", 0);
			this->lumaLabel->Text = (((float) lumaTrackBar->Value)/100.0).ToString("F");
			this->chromaLabel->Text = (((float) chromaTrackBar->Value)/100.0).ToString("F");
			this->aliasLabel->Text = (((float) aliasTrackBar->Value)/100.0).ToString("F");
			lumaState = lumaTrackBar->Value;
			chromaState = chromaTrackBar->Value;
			aliasState = aliasTrackBar->Value;
			this->compressCheckBox->Checked = registry->get("compressEnabled", false);
			compressState =  this->compressCheckBox->Checked;
			this->losslessCheckBox->Checked = registry->get("losslessEnabled", false);
			losslessState =  this->losslessCheckBox->Checked;
			this->maximizeResolutionCheckBox->Checked = registry->get("maximizeResolution", false);
			maximizeResolutionState =  this->maximizeResolutionCheckBox->Checked;
			this->appleDNGCheckBox->Checked = registry->get("appleDNG", false);
			appleDNGState =  this->appleDNGCheckBox->Checked;

		}
	public: event EventHandler^ imageApply;

	protected: virtual void OnImageApply(EventArgs^ e)
    {
            imageApply(this, e);
    }


	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ImageOptions()
		{
			 CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			 registry->set("lumaStrength", lumaTrackBar->Value);
			 registry->set("chromaStrength", chromaTrackBar->Value);
			 registry->set("antiAliasStrength", aliasTrackBar->Value);
			 registry->set("compressEnabled", compressCheckBox->Checked);
			 registry->set("losslessEnabled", losslessCheckBox->Checked);
			 registry->set("maximizeResolution", maximizeResolutionCheckBox->Checked);
			 registry->set("appleDNG", appleDNGCheckBox->Checked);
			 if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  ImageCancel;
	protected: 

	private: System::Windows::Forms::Button^  ImageOK;
	protected: 

	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  ImageApply;
	private: System::Windows::Forms::Button^  ImageDefaults;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  chromaLabel;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Label^  lumaLabel;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TrackBar^  lumaTrackBar;
	private: System::Windows::Forms::TrackBar^  chromaTrackBar;
	private: System::Windows::Forms::Label^  aliasLabel;
	private: System::Windows::Forms::TrackBar^  aliasTrackBar;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::ComponentModel::IContainer^  components;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

		int lumaState;
		int chromaState;
		bool compressState;
		bool losslessState;
		bool maximizeResolutionState;
		bool appleDNGState;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::CheckBox^  compressCheckBox;


	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::CheckBox^  losslessCheckBox;
private: System::Windows::Forms::CheckBox^  maximizeResolutionCheckBox;
private: System::Windows::Forms::CheckBox^  appleDNGCheckBox;

		 int aliasState;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ImageOptions::typeid));
			this->ImageCancel = (gcnew System::Windows::Forms::Button());
			this->ImageOK = (gcnew System::Windows::Forms::Button());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->chromaTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->chromaLabel = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->lumaTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->lumaLabel = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->aliasLabel = (gcnew System::Windows::Forms::Label());
			this->aliasTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->appleDNGCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->maximizeResolutionCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->losslessCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->compressCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->ImageApply = (gcnew System::Windows::Forms::Button());
			this->ImageDefaults = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chromaTrackBar))->BeginInit();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lumaTrackBar))->BeginInit();
			this->tabPage2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->aliasTrackBar))->BeginInit();
			this->tabPage3->SuspendLayout();
			this->SuspendLayout();
			// 
			// ImageCancel
			// 
			this->ImageCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->ImageCancel->Location = System::Drawing::Point(429, 376);
			this->ImageCancel->Name = L"ImageCancel";
			this->ImageCancel->Size = System::Drawing::Size(75, 20);
			this->ImageCancel->TabIndex = 10;
			this->ImageCancel->Text = L"Cancel";
			this->toolTip1->SetToolTip(this->ImageCancel, L"Reverts to the previous settings");
			this->ImageCancel->UseVisualStyleBackColor = true;
			this->ImageCancel->Click += gcnew System::EventHandler(this, &ImageOptions::ImageCancel_Click);
			// 
			// ImageOK
			// 
			this->ImageOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->ImageOK->Location = System::Drawing::Point(323, 376);
			this->ImageOK->Name = L"ImageOK";
			this->ImageOK->Size = System::Drawing::Size(70, 20);
			this->ImageOK->TabIndex = 9;
			this->ImageOK->Text = L"OK";
			this->toolTip1->SetToolTip(this->ImageOK, L"Accepts the current settings");
			this->ImageOK->UseVisualStyleBackColor = true;
			this->ImageOK->Click += gcnew System::EventHandler(this, &ImageOptions::ImageOK_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Top;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(515, 370);
			this->tabControl1->TabIndex = 12;
			this->toolTip1->SetToolTip(this->tabControl1, L"Adjust Luminance, Chroma and anti-aliasing");
			// 
			// tabPage1
			// 
			this->tabPage1->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage1->Controls->Add(this->groupBox3);
			this->tabPage1->Controls->Add(this->groupBox2);
			this->tabPage1->Controls->Add(this->textBox2);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(507, 344);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"  Luminance and Chroma  ";
			this->tabPage1->ToolTipText = L"Allows Chroma and lumminance settings to be adjusted";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->label9);
			this->groupBox3->Controls->Add(this->label10);
			this->groupBox3->Controls->Add(this->chromaTrackBar);
			this->groupBox3->Controls->Add(this->chromaLabel);
			this->groupBox3->Location = System::Drawing::Point(156, 7);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(126, 329);
			this->groupBox3->TabIndex = 14;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Chroma";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(84, 153);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(13, 13);
			this->label8->TabIndex = 7;
			this->label8->Text = L"1";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(84, 25);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(13, 13);
			this->label9->TabIndex = 6;
			this->label9->Text = L"2";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(84, 281);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(13, 13);
			this->label10->TabIndex = 5;
			this->label10->Text = L"0";
			// 
			// chromaTrackBar
			// 
			this->chromaTrackBar->LargeChange = 10;
			this->chromaTrackBar->Location = System::Drawing::Point(38, 19);
			this->chromaTrackBar->Maximum = 200;
			this->chromaTrackBar->Name = L"chromaTrackBar";
			this->chromaTrackBar->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->chromaTrackBar->Size = System::Drawing::Size(45, 281);
			this->chromaTrackBar->TabIndex = 1;
			this->chromaTrackBar->TickFrequency = 10;
			this->toolTip1->SetToolTip(this->chromaTrackBar, L"Chroma strength");
			this->chromaTrackBar->Scroll += gcnew System::EventHandler(this, &ImageOptions::chromaTrackBar_Scroll);
			// 
			// chromaLabel
			// 
			this->chromaLabel->AutoSize = true;
			this->chromaLabel->Location = System::Drawing::Point(40, 305);
			this->chromaLabel->Name = L"chromaLabel";
			this->chromaLabel->Size = System::Drawing::Size(28, 13);
			this->chromaLabel->TabIndex = 0;
			this->chromaLabel->Text = L"0.00";
			this->chromaLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->lumaTrackBar);
			this->groupBox2->Controls->Add(this->lumaLabel);
			this->groupBox2->Location = System::Drawing::Point(21, 6);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(107, 331);
			this->groupBox2->TabIndex = 13;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Luminance";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(71, 153);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(13, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"1";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(71, 25);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(13, 13);
			this->label6->TabIndex = 3;
			this->label6->Text = L"2";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(71, 281);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(13, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"0";
			// 
			// lumaTrackBar
			// 
			this->lumaTrackBar->LargeChange = 10;
			this->lumaTrackBar->Location = System::Drawing::Point(25, 19);
			this->lumaTrackBar->Maximum = 200;
			this->lumaTrackBar->Name = L"lumaTrackBar";
			this->lumaTrackBar->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->lumaTrackBar->Size = System::Drawing::Size(45, 282);
			this->lumaTrackBar->TabIndex = 1;
			this->lumaTrackBar->TickFrequency = 10;
			this->toolTip1->SetToolTip(this->lumaTrackBar, L"Luminance strength");
			this->lumaTrackBar->Scroll += gcnew System::EventHandler(this, &ImageOptions::lumaTrackBar_Scroll);
			// 
			// lumaLabel
			// 
			this->lumaLabel->AutoSize = true;
			this->lumaLabel->Location = System::Drawing::Point(27, 305);
			this->lumaLabel->Name = L"lumaLabel";
			this->lumaLabel->Size = System::Drawing::Size(28, 13);
			this->lumaLabel->TabIndex = 0;
			this->lumaLabel->Text = L"0.00";
			this->lumaLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBox2
			// 
			this->textBox2->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox2->Location = System::Drawing::Point(299, 25);
			this->textBox2->Multiline = true;
			this->textBox2->Name = L"textBox2";
			this->textBox2->ReadOnly = true;
			this->textBox2->Size = System::Drawing::Size(200, 313);
			this->textBox2->TabIndex = 12;
			this->textBox2->Text = resources->GetString(L"textBox2.Text");
			// 
			// tabPage2
			// 
			this->tabPage2->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage2->Controls->Add(this->groupBox1);
			this->tabPage2->Controls->Add(this->textBox1);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(507, 344);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"  Anti-Aliasing  ";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->aliasLabel);
			this->groupBox1->Controls->Add(this->aliasTrackBar);
			this->groupBox1->Location = System::Drawing::Point(29, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(118, 331);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Anti-Alias Strength";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(69, 28);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(13, 13);
			this->label4->TabIndex = 5;
			this->label4->Text = L"3";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(69, 110);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(13, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"2";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(69, 191);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(13, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"1";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(69, 276);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(13, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"0";
			// 
			// aliasLabel
			// 
			this->aliasLabel->AutoSize = true;
			this->aliasLabel->Location = System::Drawing::Point(32, 299);
			this->aliasLabel->Name = L"aliasLabel";
			this->aliasLabel->Size = System::Drawing::Size(28, 13);
			this->aliasLabel->TabIndex = 1;
			this->aliasLabel->Text = L"0.00";
			this->aliasLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// aliasTrackBar
			// 
			this->aliasTrackBar->LargeChange = 30;
			this->aliasTrackBar->Location = System::Drawing::Point(30, 21);
			this->aliasTrackBar->Maximum = 300;
			this->aliasTrackBar->Name = L"aliasTrackBar";
			this->aliasTrackBar->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->aliasTrackBar->Size = System::Drawing::Size(45, 275);
			this->aliasTrackBar->SmallChange = 5;
			this->aliasTrackBar->TabIndex = 0;
			this->aliasTrackBar->TickFrequency = 20;
			this->toolTip1->SetToolTip(this->aliasTrackBar, L"Anti-aliasing strength");
			this->aliasTrackBar->Scroll += gcnew System::EventHandler(this, &ImageOptions::aliasTrackBar_Scroll);
			// 
			// textBox1
			// 
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(292, 18);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(209, 320);
			this->textBox1->TabIndex = 0;
			this->textBox1->Text = resources->GetString(L"textBox1.Text");
			// 
			// tabPage3
			// 
			this->tabPage3->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage3->Controls->Add(this->appleDNGCheckBox);
			this->tabPage3->Controls->Add(this->maximizeResolutionCheckBox);
			this->tabPage3->Controls->Add(this->losslessCheckBox);
			this->tabPage3->Controls->Add(this->compressCheckBox);
			this->tabPage3->Controls->Add(this->textBox3);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(507, 344);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Advanced";
			// 
			// appleDNGCheckBox
			// 
			this->appleDNGCheckBox->AutoSize = true;
			this->appleDNGCheckBox->Location = System::Drawing::Point(28, 281);
			this->appleDNGCheckBox->Name = L"appleDNGCheckBox";
			this->appleDNGCheckBox->Size = System::Drawing::Size(177, 17);
			this->appleDNGCheckBox->TabIndex = 17;
			this->appleDNGCheckBox->Text = L"Enable Apple DNG Compatibility";
			this->toolTip1->SetToolTip(this->appleDNGCheckBox, L"Workaround for green cast in Aperture and Preview Apps");
			this->appleDNGCheckBox->UseVisualStyleBackColor = true;
			// 
			// maximizeResolutionCheckBox
			// 
			this->maximizeResolutionCheckBox->AutoSize = true;
			this->maximizeResolutionCheckBox->Location = System::Drawing::Point(28, 204);
			this->maximizeResolutionCheckBox->Name = L"maximizeResolutionCheckBox";
			this->maximizeResolutionCheckBox->Size = System::Drawing::Size(199, 17);
			this->maximizeResolutionCheckBox->TabIndex = 16;
			this->maximizeResolutionCheckBox->Text = L"Enable Capture One V4 Compatibility";
			this->toolTip1->SetToolTip(this->maximizeResolutionCheckBox, L"Allows non-level compressed C1 V4 files to be written");
			this->maximizeResolutionCheckBox->UseVisualStyleBackColor = true;
			// 
			// losslessCheckBox
			// 
			this->losslessCheckBox->AutoSize = true;
			this->losslessCheckBox->Location = System::Drawing::Point(28, 125);
			this->losslessCheckBox->Name = L"losslessCheckBox";
			this->losslessCheckBox->Size = System::Drawing::Size(214, 17);
			this->losslessCheckBox->TabIndex = 15;
			this->losslessCheckBox->Text = L"Enable Lossless (Huffman) Compression";
			this->losslessCheckBox->UseVisualStyleBackColor = true;
			// 
			// compressCheckBox
			// 
			this->compressCheckBox->AutoSize = true;
			this->compressCheckBox->Location = System::Drawing::Point(28, 7);
			this->compressCheckBox->Name = L"compressCheckBox";
			this->compressCheckBox->Size = System::Drawing::Size(151, 17);
			this->compressCheckBox->TabIndex = 14;
			this->compressCheckBox->Text = L"Enable Level Compression";
			this->toolTip1->SetToolTip(this->compressCheckBox, L"If checked, DNG output files are level compressed using the same compression sche" 
				L"me that the original file used");
			this->compressCheckBox->UseVisualStyleBackColor = true;
			// 
			// textBox3
			// 
			this->textBox3->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox3->Location = System::Drawing::Point(248, 10);
			this->textBox3->Multiline = true;
			this->textBox3->Name = L"textBox3";
			this->textBox3->ReadOnly = true;
			this->textBox3->Size = System::Drawing::Size(243, 328);
			this->textBox3->TabIndex = 13;
			this->textBox3->Text = resources->GetString(L"textBox3.Text");
			// 
			// ImageApply
			// 
			this->ImageApply->Location = System::Drawing::Point(188, 376);
			this->ImageApply->Name = L"ImageApply";
			this->ImageApply->Size = System::Drawing::Size(99, 20);
			this->ImageApply->TabIndex = 13;
			this->ImageApply->Text = L"Apply";
			this->toolTip1->SetToolTip(this->ImageApply, L"Applies the current settings to the displayed image without closing the dialog bo" 
				L"x");
			this->ImageApply->UseVisualStyleBackColor = true;
			this->ImageApply->Click += gcnew System::EventHandler(this, &ImageOptions::ImageApply_Click);
			// 
			// ImageDefaults
			// 
			this->ImageDefaults->Location = System::Drawing::Point(60, 376);
			this->ImageDefaults->Name = L"ImageDefaults";
			this->ImageDefaults->Size = System::Drawing::Size(92, 20);
			this->ImageDefaults->TabIndex = 14;
			this->ImageDefaults->Text = L"Defaults";
			this->toolTip1->SetToolTip(this->ImageDefaults, L"Resets all values to their default");
			this->ImageDefaults->UseVisualStyleBackColor = true;
			this->ImageDefaults->Click += gcnew System::EventHandler(this, &ImageOptions::ImageDefaults_Click);
			// 
			// ImageOptions
			// 
			this->AcceptButton = this->ImageOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->ImageCancel;
			this->ClientSize = System::Drawing::Size(515, 405);
			this->Controls->Add(this->ImageDefaults);
			this->Controls->Add(this->ImageApply);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->ImageCancel);
			this->Controls->Add(this->ImageOK);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ImageOptions";
			this->Text = L"Image Correction Options";
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chromaTrackBar))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lumaTrackBar))->EndInit();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->aliasTrackBar))->EndInit();
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void ImageCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			lumaTrackBar->Value = lumaState;
			chromaTrackBar->Value = chromaState;
			aliasTrackBar->Value = aliasState;
			this->compressCheckBox->Checked = compressState;
			this->losslessCheckBox->Checked = losslessState;
			this->maximizeResolutionCheckBox->Checked = maximizeResolutionState;
			this->appleDNGCheckBox->Checked = appleDNGState;
			this->lumaLabel->Text = (((float) lumaTrackBar->Value)/100.0).ToString("F");
			this->chromaLabel->Text = (((float) chromaTrackBar->Value)/100.0).ToString("F");
			this->aliasLabel->Text = (((float) aliasTrackBar->Value)/100.0).ToString("F");
		 }


private: System::Void ImageOK_Click(System::Object^  sender, System::EventArgs^  e) {
			lumaState = lumaTrackBar->Value;
			chromaState = chromaTrackBar->Value;
			aliasState = aliasTrackBar->Value;
			compressState = this->compressCheckBox->Checked;
			losslessState = this->losslessCheckBox->Checked;
			maximizeResolutionState = this->maximizeResolutionCheckBox->Checked;
			appleDNGState = this->appleDNGCheckBox->Checked;
		 }


private: System::Void ImageApply_Click(System::Object^  sender, System::EventArgs^  e) {
			lumaState = lumaTrackBar->Value;
			chromaState = chromaTrackBar->Value;
			aliasState = aliasTrackBar->Value;
			compressState = this->compressCheckBox->Checked;
			losslessState = this->losslessCheckBox->Checked;
			maximizeResolutionState = this->maximizeResolutionCheckBox->Checked;
			appleDNGState = this->appleDNGCheckBox->Checked;
			this->ImageCancel->Enabled = false;
			this->ImageOK->Enabled = false;
			this->ImageApply->Enabled = false;
			this->ImageDefaults->Enabled = false;
			OnImageApply(gcnew EventArgs());
		 }

public: void processingDone()
		{
			 this->ImageCancel->Enabled = true;
			 this->ImageOK->Enabled = true;
			 this->ImageApply->Enabled = true;
			 this->ImageDefaults->Enabled = true;
		}


private: System::Void ImageDefaults_Click(System::Object^  sender, System::EventArgs^  e) {
			lumaTrackBar->Value = 100;
			chromaTrackBar->Value = 100;
			aliasTrackBar->Value = 0;
			this->compressCheckBox->Checked = false;
			this->losslessCheckBox->Checked = false;
			this->maximizeResolutionCheckBox->Checked = false;
			this->appleDNGCheckBox->Checked = false;
			this->lumaLabel->Text = (((float) lumaTrackBar->Value)/100.0).ToString("F");
			this->chromaLabel->Text = (((float) chromaTrackBar->Value)/100.0).ToString("F");
			this->aliasLabel->Text = (((float) aliasTrackBar->Value)/100.0).ToString("F");
		 }


private: System::Void lumaTrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
			this->lumaLabel->Text = (((float) lumaTrackBar->Value)/100.0).ToString("F");
		 }

private: System::Void chromaTrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
			this->chromaLabel->Text = (((float) chromaTrackBar->Value)/100.0).ToString("F");
		 }

private: System::Void aliasTrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
			this->aliasLabel->Text = (((float) aliasTrackBar->Value)/100.0).ToString("F");
		 }

public: int ImageOptions::getLuma()
		{
			return lumaTrackBar->Value;
		}

public: int ImageOptions::getChroma()
		{
			return chromaTrackBar->Value;
		}

public: int ImageOptions::getAlias()
		{
			return aliasTrackBar->Value;
		}

public: bool ImageOptions::getCompress() {
			return this->compressCheckBox->Checked;
		}


public: bool ImageOptions::getLossless() {
			return this->losslessCheckBox->Checked;
		}

public: bool ImageOptions::getMaximizeResolution() {
			return this->maximizeResolutionCheckBox->Checked;
		}

public: bool ImageOptions::getAppleDNG() {
			return this->appleDNGCheckBox->Checked;
		}

private: System::Void appleDNGCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}
