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
 *
 */

/*
 * Display options for CornerFix
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
	/// Summary for DisplayOptions
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class DisplayOptions : public System::Windows::Forms::Form
	{
	public:
		DisplayOptions(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			this->verboseCheckBox->Checked = registry->get("verboseEnabled", false);
			currentState =  this->verboseCheckBox->Checked;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DisplayOptions()
		{
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			registry->set("verboseEnabled", this->verboseCheckBox->Checked);
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  verboseCheckBox;
	protected: 


	private: System::Windows::Forms::Button^  DisplayOK;

	private: System::Windows::Forms::Button^  DisplayCancel;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::TextBox^  textBox1;


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

		bool currentState;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->verboseCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->DisplayOK = (gcnew System::Windows::Forms::Button());
			this->DisplayCancel = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// verboseCheckBox
			// 
			this->verboseCheckBox->AutoSize = true;
			this->verboseCheckBox->Location = System::Drawing::Point(19, 30);
			this->verboseCheckBox->Name = L"verboseCheckBox";
			this->verboseCheckBox->Size = System::Drawing::Size(115, 17);
			this->verboseCheckBox->TabIndex = 0;
			this->verboseCheckBox->Text = L"Verbose messages";
			this->toolTip1->SetToolTip(this->verboseCheckBox, L"If checked, detailed information on the DNG file loaded and CornerFix\'s internal " 
				L"operations will be sent to the text pane. The default is off.");
			this->verboseCheckBox->UseVisualStyleBackColor = true;
			// 
			// DisplayOK
			// 
			this->DisplayOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->DisplayOK->Location = System::Drawing::Point(154, 268);
			this->DisplayOK->Name = L"DisplayOK";
			this->DisplayOK->Size = System::Drawing::Size(70, 20);
			this->DisplayOK->TabIndex = 2;
			this->DisplayOK->Text = L"OK";
			this->toolTip1->SetToolTip(this->DisplayOK, L"Accept displayed values");
			this->DisplayOK->UseVisualStyleBackColor = true;
			this->DisplayOK->Click += gcnew System::EventHandler(this, &DisplayOptions::DisplayOK_Click);
			// 
			// DisplayCancel
			// 
			this->DisplayCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->DisplayCancel->Location = System::Drawing::Point(261, 268);
			this->DisplayCancel->Name = L"DisplayCancel";
			this->DisplayCancel->Size = System::Drawing::Size(75, 20);
			this->DisplayCancel->TabIndex = 3;
			this->DisplayCancel->Text = L"Cancel";
			this->toolTip1->SetToolTip(this->DisplayCancel, L"Revert to previous values");
			this->DisplayCancel->UseVisualStyleBackColor = true;
			this->DisplayCancel->Click += gcnew System::EventHandler(this, &DisplayOptions::DisplayCancel_Click);
			// 
			// textBox1
			// 
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(157, 32);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(178, 222);
			this->textBox1->TabIndex = 4;
			this->textBox1->Text = L"If verbose is enabled, detailed information of the DNG file opened and the progre" 
				L"ss of CornerFix operations will be dispalyed\r\n\r\nNote that enabling this slows do" 
				L"wn CornerFix considerably.";
			// 
			// DisplayOptions
			// 
			this->AcceptButton = this->DisplayOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->CancelButton = this->DisplayCancel;
			this->ClientSize = System::Drawing::Size(348, 299);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->DisplayCancel);
			this->Controls->Add(this->DisplayOK);
			this->Controls->Add(this->verboseCheckBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"DisplayOptions";
			this->Text = L"Display Options";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void DisplayCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->verboseCheckBox->Checked = currentState;
			 this->Hide();
		 }

private: System::Void DisplayOK_Click(System::Object^  sender, System::EventArgs^  e) {
			 currentState = this->verboseCheckBox->Checked;
			 this->Hide();
		 }

public: bool DisplayOptions::GetVerbose() {
			return this->verboseCheckBox->Checked;
		}

};
}
