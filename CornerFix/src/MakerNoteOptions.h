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
 * MakerNoteOptions.h
 * ---------------
 * (C) Copyright 2007-2009, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 23 Oct 2007 : Original Version
 * 29 Nov 2007 : 0.9.1.7
 *
 */

/*
 * MakerNote options for CornerFix
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
	/// Summary for MakerNoteOptions
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MakerNoteOptions : public System::Windows::Forms::Form
	{
	public:
		MakerNoteOptions(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			this->ExifUpdateCheckBox->Checked = registry->get("exifUpdateEnabled", false);
			exifUpdateState = this->ExifUpdateCheckBox->Checked;
			this->CalculatedApetureCheckBox->Checked = registry->get("calculatedApertureEnabled", true);
			calculatedApertureState = this->CalculatedApetureCheckBox->Checked;
			this->ClipAperturesCheckBox->Checked = registry->get("clipAperturesEnabled", true);
			ClipAperturesState = this->ClipAperturesCheckBox->Checked;

			this->CalculatedApetureCheckBox->Enabled = this->ExifUpdateCheckBox->Checked;
			this->ClipAperturesCheckBox->Enabled = this->ExifUpdateCheckBox->Checked;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MakerNoteOptions()
		{
			CornerFixRegistry^ registry = gcnew CornerFixRegistry();	
			registry->set("exifUpdateEnabled", this->ExifUpdateCheckBox->Checked);
			registry->set("calculatedApertureEnabled", this->CalculatedApetureCheckBox->Checked);
			registry->set("clipAperturesEnabled", this->ClipAperturesCheckBox->Checked);
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  MakerNotesCancel;
	protected: 

	protected: 
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::Button^  MakerNotesOK;

	private: System::Windows::Forms::CheckBox^  ExifUpdateCheckBox;


	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::CheckBox^  ClipAperturesCheckBox;
	private: System::Windows::Forms::CheckBox^  CalculatedApetureCheckBox;

	private:
		bool exifUpdateState;
		bool ClipAperturesState;
		bool calculatedApertureState;

		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MakerNoteOptions::typeid));
			this->MakerNotesCancel = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->MakerNotesOK = (gcnew System::Windows::Forms::Button());
			this->ExifUpdateCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->ClipAperturesCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->CalculatedApetureCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// MakerNotesCancel
			// 
			this->MakerNotesCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->MakerNotesCancel->Location = System::Drawing::Point(282, 255);
			this->MakerNotesCancel->Name = L"MakerNotesCancel";
			this->MakerNotesCancel->Size = System::Drawing::Size(75, 20);
			this->MakerNotesCancel->TabIndex = 12;
			this->MakerNotesCancel->Text = L"Cancel";
			this->toolTip1->SetToolTip(this->MakerNotesCancel, L"Revert to previous values");
			this->MakerNotesCancel->UseVisualStyleBackColor = true;
			this->MakerNotesCancel->Click += gcnew System::EventHandler(this, &MakerNoteOptions::MakerNotesCancel_Click);
			// 
			// MakerNotesOK
			// 
			this->MakerNotesOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->MakerNotesOK->Location = System::Drawing::Point(175, 255);
			this->MakerNotesOK->Name = L"MakerNotesOK";
			this->MakerNotesOK->Size = System::Drawing::Size(70, 20);
			this->MakerNotesOK->TabIndex = 11;
			this->MakerNotesOK->Text = L"OK";
			this->toolTip1->SetToolTip(this->MakerNotesOK, L"Accept displayed values");
			this->MakerNotesOK->UseVisualStyleBackColor = true;
			this->MakerNotesOK->Click += gcnew System::EventHandler(this, &MakerNoteOptions::MakerNotesOK_Click);
			// 
			// ExifUpdateCheckBox
			// 
			this->ExifUpdateCheckBox->AutoSize = true;
			this->ExifUpdateCheckBox->Location = System::Drawing::Point(14, 12);
			this->ExifUpdateCheckBox->Name = L"ExifUpdateCheckBox";
			this->ExifUpdateCheckBox->Size = System::Drawing::Size(87, 17);
			this->ExifUpdateCheckBox->TabIndex = 10;
			this->ExifUpdateCheckBox->Text = L"Update EXIF";
			this->toolTip1->SetToolTip(this->ExifUpdateCheckBox, L"If enabled, the image\'s EFIF data is updated from the MakerNotes");
			this->ExifUpdateCheckBox->UseVisualStyleBackColor = true;
			this->ExifUpdateCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MakerNoteOptions::ExifUpdateCheckBox_CheckedChanged);
			// 
			// ClipAperturesCheckBox
			// 
			this->ClipAperturesCheckBox->AutoSize = true;
			this->ClipAperturesCheckBox->Location = System::Drawing::Point(14, 168);
			this->ClipAperturesCheckBox->Name = L"ClipAperturesCheckBox";
			this->ClipAperturesCheckBox->Size = System::Drawing::Size(119, 17);
			this->ClipAperturesCheckBox->TabIndex = 14;
			this->ClipAperturesCheckBox->Text = L"Clip aperture values";
			this->toolTip1->SetToolTip(this->ClipAperturesCheckBox, L"If enabled, the estimated aperture values are limited to those possible for the l" 
				L"ens detected");
			this->ClipAperturesCheckBox->UseVisualStyleBackColor = true;
			// 
			// CalculatedApetureCheckBox
			// 
			this->CalculatedApetureCheckBox->AutoSize = true;
			this->CalculatedApetureCheckBox->Location = System::Drawing::Point(14, 103);
			this->CalculatedApetureCheckBox->Name = L"CalculatedApetureCheckBox";
			this->CalculatedApetureCheckBox->Size = System::Drawing::Size(139, 17);
			this->CalculatedApetureCheckBox->TabIndex = 15;
			this->CalculatedApetureCheckBox->Text = L"Use calculated aperture";
			this->toolTip1->SetToolTip(this->CalculatedApetureCheckBox, L"If enabled, CornerFix\'s calculated estimate for aperture is used. Else the leica " 
				L"estimate is used");
			this->CalculatedApetureCheckBox->UseVisualStyleBackColor = true;
			// 
			// textBox1
			// 
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(168, 14);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(200, 235);
			this->textBox1->TabIndex = 13;
			this->textBox1->Text = resources->GetString(L"textBox1.Text");
			// 
			// MakerNoteOptions
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(382, 287);
			this->Controls->Add(this->CalculatedApetureCheckBox);
			this->Controls->Add(this->ClipAperturesCheckBox);
			this->Controls->Add(this->MakerNotesCancel);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->MakerNotesOK);
			this->Controls->Add(this->ExifUpdateCheckBox);
			this->Name = L"MakerNoteOptions";
			this->Text = L"MakerNote Options";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void MakerNotesOK_Click(System::Object^  sender, System::EventArgs^  e) {
			exifUpdateState = this->ExifUpdateCheckBox->Checked;
			calculatedApertureState = this->CalculatedApetureCheckBox->Checked;
			ClipAperturesState = this->ClipAperturesCheckBox->Checked;
		 }
private: System::Void MakerNotesCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			this->ExifUpdateCheckBox->Checked = exifUpdateState;
			this->CalculatedApetureCheckBox->Checked = calculatedApertureState;
			this->ClipAperturesCheckBox->Checked = ClipAperturesState;
		 }
private: System::Void ExifUpdateCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			this->CalculatedApetureCheckBox->Enabled = this->ExifUpdateCheckBox->Checked;
			this->ClipAperturesCheckBox->Enabled = this->ExifUpdateCheckBox->Checked;
		 }
public: bool MakerNoteOptions::GetExifUpdate() {
			return this->ExifUpdateCheckBox->Checked;
		}
public: bool MakerNoteOptions::GetCalculatedAperture() {
			return this->CalculatedApetureCheckBox->Checked;
		}
public: bool MakerNoteOptions::GetClipApertures() {
			return this->ClipAperturesCheckBox->Checked;
		}
};
}
