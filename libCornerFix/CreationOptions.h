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
 * CreationOptions.h
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
 * 1 Jul 2007 : 0.9.1.0
 * 14 Oct 2009 : 1.3.0.0
 *
 */

/*
 * Profile creation options for CornerFix
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
	/// Summary for CreationOptions
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CreationOptions : public System::Windows::Forms::Form
	{
	public:
		CreationOptions(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			this->multipleCheckBox->Checked = registry->get("multipleEquationEnabled", false);
			this->LuminanceCheckBox->Checked = registry->get("luminanceCompensationEnabled", true);
			this->bidirectionalCheckBox->Checked = registry->get("bidirectionalCompensationEnabled", true);
			multipleState = this->multipleCheckBox->Checked;
			illuminantState = this->LuminanceCheckBox->Checked;
			bidirectionalState = this->bidirectionalCheckBox->Checked;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CreationOptions()
		{
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			registry->set("multipleEquationEnabled", this->multipleCheckBox->Checked);
			registry->set("luminanceCompensationEnabled", this->LuminanceCheckBox->Checked);
			registry->set("bidirectionalCompensationEnabled", this->bidirectionalCheckBox->Checked);
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  CreationCancel;
	protected: 

	private: System::Windows::Forms::Button^  CreationOK;
	private: System::Windows::Forms::CheckBox^  multipleCheckBox;
	private: System::Windows::Forms::CheckBox^  LuminanceCheckBox;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::CheckBox^  bidirectionalCheckBox;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

		bool multipleState;
		bool illuminantState;
		bool bidirectionalState;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CreationOptions::typeid));
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->CreationCancel = (gcnew System::Windows::Forms::Button());
			this->CreationOK = (gcnew System::Windows::Forms::Button());
			this->multipleCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->LuminanceCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->bidirectionalCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(177, 12);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(217, 279);
			this->textBox1->TabIndex = 8;
			this->textBox1->Text = resources->GetString(L"textBox1.Text");
			// 
			// CreationCancel
			// 
			this->CreationCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CreationCancel->Location = System::Drawing::Point(291, 297);
			this->CreationCancel->Name = L"CreationCancel";
			this->CreationCancel->Size = System::Drawing::Size(75, 20);
			this->CreationCancel->TabIndex = 7;
			this->CreationCancel->Text = L"Cancel";
			this->toolTip1->SetToolTip(this->CreationCancel, L"Revert to previous values");
			this->CreationCancel->UseVisualStyleBackColor = true;
			this->CreationCancel->Click += gcnew System::EventHandler(this, &CreationOptions::CreationCancel_Click);
			// 
			// CreationOK
			// 
			this->CreationOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->CreationOK->Location = System::Drawing::Point(184, 297);
			this->CreationOK->Name = L"CreationOK";
			this->CreationOK->Size = System::Drawing::Size(70, 20);
			this->CreationOK->TabIndex = 6;
			this->CreationOK->Text = L"OK";
			this->toolTip1->SetToolTip(this->CreationOK, L"Accept displayed values");
			this->CreationOK->UseVisualStyleBackColor = true;
			this->CreationOK->Click += gcnew System::EventHandler(this, &CreationOptions::CreationOK_Click);
			// 
			// multipleCheckBox
			// 
			this->multipleCheckBox->AutoSize = true;
			this->multipleCheckBox->Location = System::Drawing::Point(23, 10);
			this->multipleCheckBox->Name = L"multipleCheckBox";
			this->multipleCheckBox->Size = System::Drawing::Size(111, 17);
			this->multipleCheckBox->TabIndex = 5;
			this->multipleCheckBox->Text = L"Multiple equations";
			this->toolTip1->SetToolTip(this->multipleCheckBox, L"If enabled, all preprogrammed vignetting equations are evaluated");
			this->multipleCheckBox->UseVisualStyleBackColor = true;
			// 
			// LuminanceCheckBox
			// 
			this->LuminanceCheckBox->AutoSize = true;
			this->LuminanceCheckBox->Checked = true;
			this->LuminanceCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->LuminanceCheckBox->Location = System::Drawing::Point(24, 89);
			this->LuminanceCheckBox->Name = L"LuminanceCheckBox";
			this->LuminanceCheckBox->Size = System::Drawing::Size(147, 17);
			this->LuminanceCheckBox->TabIndex = 9;
			this->LuminanceCheckBox->Text = L"Luminance compensation";
			this->toolTip1->SetToolTip(this->LuminanceCheckBox, L"If enabled, CornerFix compensates for luminance variations on the reference image" 
				L"");
			this->LuminanceCheckBox->UseVisualStyleBackColor = true;
			// 
			// bidirectionalCheckBox
			// 
			this->bidirectionalCheckBox->AutoSize = true;
			this->bidirectionalCheckBox->Location = System::Drawing::Point(24, 180);
			this->bidirectionalCheckBox->Name = L"bidirectionalCheckBox1";
			this->bidirectionalCheckBox->Size = System::Drawing::Size(134, 17);
			this->bidirectionalCheckBox->TabIndex = 10;
			this->bidirectionalCheckBox->Text = L"Bidirectional Correction";
			this->toolTip1->SetToolTip(this->bidirectionalCheckBox, L"Disable Birectional mode if over-compensation occurs e.g., \"pink corners\". ");
			this->bidirectionalCheckBox->UseVisualStyleBackColor = true;
			// 
			// CreationOptions
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(403, 329);
			this->Controls->Add(this->bidirectionalCheckBox);
			this->Controls->Add(this->LuminanceCheckBox);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->CreationCancel);
			this->Controls->Add(this->CreationOK);
			this->Controls->Add(this->multipleCheckBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"CreationOptions";
			this->Text = L"Profile Creation Options";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void CreationOK_Click(System::Object^  sender, System::EventArgs^  e) {
			multipleState = this->multipleCheckBox->Checked;
			illuminantState = this->LuminanceCheckBox->Checked;
			bidirectionalState = this->bidirectionalCheckBox->Checked;
			 }
	private: System::Void CreationCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			this->multipleCheckBox->Checked = multipleState;
			this->LuminanceCheckBox->Checked = illuminantState;
			this->bidirectionalCheckBox->Checked = bidirectionalState;
			 }
public: bool CreationOptions::GetMultiple() {
			return this->multipleCheckBox->Checked;
		}

public: bool CreationOptions::GetIlluminant() {
			return this->LuminanceCheckBox->Checked;
		}

public: bool CreationOptions::GetBidirectional() {
			return this->bidirectionalCheckBox->Checked;
		}

};
}
