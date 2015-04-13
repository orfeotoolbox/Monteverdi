/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbMonteverdiViewGUI.h"
#include "ConfigureMonteverdi.h"

#include <iostream>
#include <cassert>

#include <FL/fl_ask.H>

#include "otbMacro.h"

#include "otbMsgReporter.h"
#include "otbCachingPathManager.h"

#include "itkExceptionObject.h"
#include "flu_pixmaps.h"
#include "otbMonteverdiPixmaps.h"

namespace otb
{
Fl_Pixmap blue_dot((char*const*) bluedot_xpm),
green_dot((char*const*) greendot_xpm),
red_dot((char*const*) reddot_xpm),
teal_dot((char*const*) tealdot_xpm),
text_doc((char*const*) textdoc_xpm),
computer((char*const*) computer_xpm),
book((char*const*) book_xpm),
cd_drive((char*const*) cd_drive_xpm),
arrow_closed((char*const*) arrow_closed_xpm),
arrow_open((char*const*) arrow_open_xpm),
home((char*const*) home_xpm),
purple_dot((char*const*) purpledot_xpm),
vectorImage((char*const*) vectorImage_xpm),
scalarImage((char*const*) scalarImage_xpm),
process    ((char*const*) process_xpm),
vectorData ((char*const*) vectorData_xpm),
pleiades ((char*const*) pleiades_xpm);

MonteverdiViewGUI
::MonteverdiViewGUI()
{
  // Build the structure of the GUI (MonteverdiViewGroup)
  this->Build();

  // Main window title includes the version
  std::ostringstream oss;
  oss << "Monteverdi - ";
  oss << Monteverdi_VERSION_STRING;
  wMainWindow->copy_label(oss.str().c_str());

  // Files Tree
  m_Tree = new FluTreeBrowser(gTreeGroup->x(), gTreeGroup->y(), gTreeGroup->w(), gTreeGroup->h(), "Tree ");
  m_Tree->insertion_mode(FLU_INSERT_BACK);

  // Initialize DnD box and register to this class to receive its notifications
  // 0.95 to avoid scrollbar deactivation due to Fl_Move event
  // In Fl_DND_Box the Fl_Move event is catched to avoid segfaults
  // (begin dragging and dropping nothing in the DND area)
  // the 0.95 decrease the DND area to not take the scrollbar into account
  m_DnDBox = new Fl_DND_Box(gTreeGroup->x(), gTreeGroup->y(), 0.95*gTreeGroup->w(), gTreeGroup->h(), "");
  m_DnDBox->RegisterListener(this);

  m_MonteverdiModel = MonteverdiModel::GetInstance();
  m_MonteverdiModel->RegisterListener(this);

  m_HelpTextBuffer = new Fl_Text_Buffer();

  m_BranchNameInfo = "Informations";
}

MonteverdiViewGUI
::~MonteverdiViewGUI()
{
  delete m_HelpTextBuffer;
  delete m_DnDBox;
}

void
MonteverdiViewGUI
::InitWidgets()
{
  assert(m_MonteverdiController != NULL && "The controller is not created");

  // Generate dynamicaly the menus with the Model informations
  this->BuildMenus();

  // Generate dynamicaly the tree
  this->BuildTree();

  gHelpText->buffer(m_HelpTextBuffer);
  gHelpText->insert("This is the application Monteverdi\n");
  gHelpText->insert("======================\n\n");
  gHelpText->insert("Configuration informations\n");
  gHelpText->insert("======================\n\n");
  gHelpText->insert("Cached data are saved in the directory ");
  gHelpText->insert(CachingPathManager::GetInstance()->GetFullCachingPath().c_str());
  gHelpText->insert("\n");
  gHelpText->insert("======================\n\n");
  gHelpText->insert("Monteverdi is an application for capacity building to provide simple\n");
  gHelpText->insert("remote sensing data analysis tools for non-experienced users.\n\n");
  gHelpText->insert("This small help box explains the basic behaviour of the application.\n\n");
  gHelpText->insert("1.  Different tools can be accessed using the menu.\n");
  gHelpText->insert("2.  Data produced using tools will appear in the tree browser under\n");
  gHelpText->insert("    the menu.\n");
  gHelpText->insert("3.  When a tool requires inputs, an input selection window will appear \n");
  gHelpText->insert("    so as to allow the user to select inputs among data he has already\n");
  gHelpText->insert("    produced using other tools. \n");
  gHelpText->insert("4.  By default, data are streamed (meaning they are split and processed\n");
  gHelpText->insert("    piece by piece) from one module to the other. \n");
  gHelpText->insert("5.  Though this behaviour is rather efficient to build a processing chain,\n");
  gHelpText->insert("    sometimes it is better to have the data \"cached\" (written to disk)\n");
  gHelpText->insert("    before doing something with it. This way, instead of computing the\n");
  gHelpText->insert("    requested data each time it is needed, computing is only done once\n");
  gHelpText->insert("    for the whole dataset and written to disk. \n");
  gHelpText->insert("6.  In the input selection window, dataset supporting caching will\n");
  gHelpText->insert("    automatically display their state (streamed/caching in progress/cached)\n");
  gHelpText->insert("    through a litle red/orange/green button aside. \n");
  gHelpText->insert("7.  If dataset is already cached, the state button will be green and deactivated.\n");
  gHelpText->insert("8.  If dataset can be cached but is not for now, the state button will be red and \n");
  gHelpText->insert("    activated. Pushing this button will trigger the caching process. A progress-bar\n");
  gHelpText->insert("    showing caching progress appears, and the state button turns orange. \n");
  gHelpText->insert("9.  Once a dataset is cached, some tools will significantly speed-up (in particular\n");
  gHelpText->insert("    tools implying dataset navigation).\n");
  gHelpText->insert("10. Be sure to exit the application properly so as to get the cached data cleared\n");
  gHelpText->insert("    from your disk. \n");
  gHelpText->insert("11. Some tools (like writing tools for instance) require to lock every other tool\n");
  gHelpText->insert("    they are plugged to so as ensure that they can complete their task safely.\n");
  gHelpText->insert("    Dataset from a locked module will appear in grey with \"(locked by module [module_name]\")\n");
  gHelpText->insert("    in its name. \n");
  gHelpText->insert("12. For application safety reasons, you can not unlock a dataset by yourself.\n");
  gHelpText->insert("    If there is a task in progress, wait for it to complete. If the dataset\n");
  gHelpText->insert("    is locked by an interactive tool, consider exiting this tool and try again.\n\n");
  gHelpText->insert("We hope you will enjoy using this application.\n \n");
  gHelpText->insert("We are interested in your feedback! If you: \n");
  gHelpText->insert("  - found what looks like a bug;\n");
  gHelpText->insert("  - Do not understand the behaviour of the application or of a given tool;\n");
  gHelpText->insert("  - Have suggestions and feature requests;\n");
  gHelpText->insert("  - Are enjoying the application so much that you want to share it with somebody;\n");
  gHelpText->insert("  - Are being driven mad by the application behaviour and want to share it with somebody.\n\n");
  gHelpText->insert("Please write to otb-users@googlegroups.com. We will be glad to answer you.");
}

/** First step of Init Widgets: creation of the menus */
void
MonteverdiViewGUI
::BuildMenus()
{
  const ModuleDescriptorMapType&          lModuleDescriptorMap = m_MonteverdiModel->GetRegisteredModuleDescriptors();
  ModuleDescriptorMapType::const_iterator mcIt;

  mMenuBar->add("File", 0, 0, 0, FL_SUBMENU);

  unsigned int idx = 0;

  while (idx < m_MonteverdiModel->GetNumberOfRegisteredModules())
    {
    mcIt = lModuleDescriptorMap.begin();

    while (mcIt != lModuleDescriptorMap.end() && (mcIt->second.m_RegistrationOrder != idx))
      {
      mcIt++;
      }
    if (mcIt !=  lModuleDescriptorMap.end())
      {
      mMenuBar->add(mcIt->second.m_MenuPath.c_str(), 0, (Fl_Callback *) MonteverdiViewGUI::GenericCallback,
                    (void *) (mcIt->second.m_Key.c_str()));
      }
    ++idx;
    }

  // In the end
  mMenuBar->add("File/Quit", 0, (Fl_Callback *) MonteverdiViewGUI::QuitCallback, (void*) (this));
  mMenuBar->add("?/Help", 0, (Fl_Callback *) MonteverdiViewGUI::HelpCallback, (void*) (this));
}

/** Second step of Init Widgets : creation of the tree */
void
MonteverdiViewGUI
::BuildTree()
{
  // FileGroup and tree
  gTreeGroup->add(m_Tree);

  m_Tree->box(FL_DOWN_BOX);
  m_Tree->auto_branches(true);
  m_Tree->label("Datasets Browser");

  // allow callback with the tree
  m_Tree->box(FL_DOWN_BOX);
  m_Tree->allow_dnd(true);

  //m_Tree->when( FL_WHEN_RELEASE );
  m_Tree->callback(TreeCallback);

  // animate the tree
  m_Tree->animate(true);
  m_Tree->collapse_time(0.02);
  m_Tree->frame_rate(500);

  //Flu_Tree_Browser::Node* root = m_Tree->first();
  FluTreeBrowser::Node* root = m_Tree->first();
  root->open(true);
  root->label("Dataset");

  gTreeGroup->resizable(m_Tree);
  wMainWindow->resizable(gTreeGroup);
  m_Tree->show_root(false);

  // Build the DND Box, actually only add it to the view gui
  wMainWindow->add(m_DnDBox);
}

/** BuildInputsGUI create an instance of a small GUI where the user will select his inputs
  * The number and the kind of inputs will be choosen considering the expectation of a
  * concerned module (moduleInstanceId)
  * Note : when no input is required, the GUI must not appear! ( -> skip )
  */
void
MonteverdiViewGUI
::BuildInputsGUI(const std::string& moduleInstanceId)
{

  bool skip  = false;

  // look after all expected or optionnal input datas
  InputDataDescriptorMapType lInputDataMap = m_MonteverdiModel->GetModuleInputsByInstanceId(moduleInstanceId);
  if (lInputDataMap.size() == 0)
    {
    skip = true;
    }

  if (!skip)
    {
    m_InputViewGUI = InputViewGUI::New();
    m_InputViewGUI->SetModel(m_MonteverdiModel);
    m_InputViewGUI->SetController(m_MonteverdiController);
    m_InputViewGUI->SetModuleInstanceId(moduleInstanceId);
    m_InputViewGUI->BuildInputInterface();
    m_InputViewGUI->Show();
    }
  else
    {
    m_MonteverdiController->StartModuleByInstanceId(moduleInstanceId);
    }

}

/** GenericCallback (static)
  *
  * Because this method is called from a button into the Fl_Menu_Bar (cf. BuildMenus),
  * "CreateModuleByKey_Callback" must be static. Problem : in this method must use
  * "this" which is not static !
  */
void
MonteverdiViewGUI
::GenericCallback(Fl_Menu_* w, void* v)
{
  MonteverdiViewGUI * pthis = static_cast<MonteverdiViewGUI *>(w->parent()->user_data());
  const char*         moduleKey = static_cast<const char *>(v);

  // each call to this callback create a new instance of a module
  pthis->CreateModuleByKey(moduleKey);
}

/** QuitCallback (static) */
void MonteverdiViewGUI::QuitCallback(Fl_Menu_* o, void* v)
{
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *) v;
  lThis->OpenEraseCaching();
}

/** HelpCallback (static) */
void MonteverdiViewGUI::HelpCallback(Fl_Menu_* o, void* v)
{
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *) v;
  lThis->Help();
}

/** TreeCallback (static) */
void MonteverdiViewGUI::TreeCallback(Fl_Widget* w, void* v)
{
  FluTreeBrowser *      t = (FluTreeBrowser*) w;
  int                   reason = t->callback_reason();
  FluTreeBrowser::Node *n = t->callback_node();

  MonteverdiViewGUI * pthis =
    dynamic_cast<MonteverdiViewGUI *>(static_cast<MonteverdiViewGroup *>(w->parent()->parent()->user_data()));

  switch (reason)
    {
    case OTB_FLU_RIGHT_MOUSE_PUSHED:
      pthis->LaunchPopupMenu(n);
      break;
    }
}

void
MonteverdiViewGUI
::LaunchPopupMenu(FluTreeBrowser::Node * n)
{
  if (n->is_root())
    {
    return;
    }

  const char * label = n->label();

  // node is a module
  if (n->parent()->is_root())
    {
    m_Tree->GetModuleMenu()->Reset();

    std::string moduleId   = n->label();

    // Look for the instance id
    ModuleMapType modMap = m_MonteverdiModel->GetModuleMap();
    ModuleMapType::const_iterator mcIt =  modMap.begin();
    ModuleMapType::const_iterator mcItEnd = modMap.end();

    for(; mcIt!= mcItEnd; mcIt++)
      {
        if( (*mcIt)->GetInstanceId() == moduleId)
          break;
      }
    if( mcIt == mcItEnd )
      {
        itkExceptionMacro(<< "No module found with instanceId " << moduleId << ".");
      }
    
    bool canShowModule = (*mcIt)->CanShow();
    m_Tree->GetModuleMenu()->LaunchModuleMenu(canShowModule);
    if (m_Tree->GetModuleMenu()->GetModuleMenuOutput() == RENAME_MODULE)
      {
      gRenameOld->value(label);
      gRenameNew->value(label);
      wRenameWindow->show();
      }

    //Show the module with current parameters
    if (m_Tree->GetModuleMenu()->GetModuleMenuOutput() == (SHOW_MODULE && canShowModule))
      {
        //Call controller?
        (*mcIt)->Show();
      }
    }
  // node is a output
  else if ( n->parent()->parent()->is_root() ||
            n->parent()->parent()->parent()->is_root() ||
            n->parent()->parent()->parent()->parent()->is_root() )
    {
    std::string instanceId;
    std::string outputId = n->label();
    // We exclude the case of informations node
    if (outputId == m_BranchNameInfo || n->parent()->label() == m_BranchNameInfo)
      {
      return;
      }
    // To retrieve the good instanceId we need to find where we are in the tree
    if (n->parent()->parent()->is_root())
      {
      instanceId = n->parent()->label();
      }
    else if (n->parent()->parent()->parent()->is_root())
      {
      instanceId = n->parent()->parent()->label();
      }
    else if (n->parent()->parent()->parent()->parent()->is_root())
      {
      instanceId = n->parent()->parent()->parent()->label();
      }
    else
      {
      return;
      }

    bool cacheable = m_MonteverdiModel->SupportsCaching(instanceId, outputId)
                    && !m_MonteverdiModel->IsCached(instanceId, outputId);
    bool viewable = m_MonteverdiModel->SupportsViewing(instanceId, outputId);
    bool writable = m_MonteverdiModel->SupportsWriting(instanceId, outputId);

    m_Tree->GetModuleMenu()->Reset();
    m_Tree->GetModuleMenu()->LaunchOutputMenu(viewable, cacheable, writable);
    if (m_Tree->GetModuleMenu()->GetOutputMenuOutput() == RENAME_OUTPUT)
      {
      std::string rootPath = n->find_path();
      // erase the end "/"
      rootPath = rootPath.substr(0, rootPath.size() - 1);
      // extract the path
      rootPath = rootPath.substr(0, rootPath.find_last_of("/") + 1);
      gOutputRenameRoot->value(rootPath.c_str());
      gOutputRenameOld->value(label);
      gOutputRenameNew->value(label);
      wOutputRenameWindow->show();
      }
    else if (m_Tree->GetModuleMenu()->GetOutputMenuOutput() == DISPLAY_OUTPUT)
      {
      m_MonteverdiController->StartViewing(instanceId, outputId);
      }
    else if (m_Tree->GetModuleMenu()->GetOutputMenuOutput() == CACHE_OUTPUT)
      {
      m_MonteverdiController->StartCaching(instanceId, outputId, true);
      }
    else if (m_Tree->GetModuleMenu()->GetOutputMenuOutput() == WRITE_OUTPUT)
      {
      m_MonteverdiController->StartWriting(instanceId, outputId);
      }
    }
  else if (n->is_leaf())
    {
    }
  return;
}

void
MonteverdiViewGUI
::RenameOk()
{
  std::string oldId =  gRenameOld->value();
  std::string newId =  gRenameNew->value();
  m_MonteverdiController->ChangeInstanceId(oldId, newId);
  this->ReplaceInTree(oldId, newId);
  wRenameWindow->hide();
}

void
MonteverdiViewGUI
::OutputRenameOk()
{
  std::string rootPath = gOutputRenameRoot->value();
  std::string oldId =  gOutputRenameOld->value();
  std::string fullOldId = oldId;
  std::string newId =  gOutputRenameNew->value();
  // Get module instanceId thanks to the full path root, first eliminate the start "/"
  std::string instanceId = rootPath.substr(1, rootPath.size());
  // keep the first chain
  instanceId = instanceId.substr(0, instanceId.find_first_of("/"));
  // replace uses find_full_path thus just the label can't work
  fullOldId.insert(0, rootPath);
  m_MonteverdiController->ChangeOutputDataKey(instanceId, oldId, newId);
  this->ReplaceInTree(fullOldId, newId);
  wOutputRenameWindow->hide();
}

void
MonteverdiViewGUI
::CreateModuleByKey(const char * modulekey)
{
  m_MonteverdiController->CreateModuleByKey(modulekey);
}

/** The tree is updated when a notifaction is received with the Event type "Output" */
void
MonteverdiViewGUI
::UpdateTree(const std::string& instanceId)
{
  FluTreeBrowser::Node* root = m_Tree->first();

  //Clear existing Module outputs before adding new entries
//   for (unsigned int i=0; i<n->children(); ++i)
//   {
//     otbGenericMsgDebugMacro( << "remove node " << n->child(i)->label() );
//     n->remove(n->child(i));
//   }
//Remove existing module with the same name in the tree
  unsigned int rm = root->remove(m_Tree->find(instanceId.c_str()));
  if (rm != 0)
    {
    otbGenericMsgDebugMacro(<< "Remove existing module entry: " << instanceId.c_str());
    }
  // add a new branch for a new instance of module
  root->add_branch(instanceId.c_str());
  root->label("Data Set");

  // look after all outputdatas into each instance of module
  OutputDataDescriptorMapType                 lDataMap = m_MonteverdiModel->GetModuleOutputsByInstanceId(instanceId);
  OutputDataDescriptorMapType::const_iterator it;

  // we look for the good node in the tree to add leaves
  FluTreeBrowser::Node* n = m_Tree->find(instanceId.c_str());
  if (!n) n = m_Tree->last();
  n->open(true);
  n->movable(true);

  // this node can receive new nodes as a result of draggind-and-dropping
  //TODO
  //n->droppable(true);

  FluTreeBrowser::Node* new_node;
  FluTreeBrowser::Node* new_node_info;

  it = lDataMap.begin();
  int countIt = 0;
  while(it != lDataMap.end())
    {
    otbMsgDevMacro( << static_cast<OutputDataDescriptor>(*it));

    // Is it a vector image ?
    if (it->GetDataType() == "Floating_Point_VectorImage"
        || it->GetDataType() == "Labeled_Char_VectorImage"
        || it->GetDataType() == "Floating_Point_Complex_VectorImage")
      {
      new_node = n->add_branch(it->GetDataKey().c_str());
      new_node->parent()->open(true);

      new_node_info = new_node->add_branch(m_BranchNameInfo.c_str());
      new_node_info->add(it->GetDataDescription().c_str());
      new_node_info->add(it->GetDataType().c_str());
      new_node_info->branch_icons(&book, &book);

      it++;
      countIt++;
      // keep some infos
      std::string prevDataType = it->GetDataType().c_str();

      // main vector image is added, so we need to add these bands
      FluTreeBrowser::Node* new_node_sub;
      while ( (it != lDataMap.end()) && (it->GetDataType() != "Floating_Point_VectorImage")
            && ( it->GetDataType() != "Labeled_Char_VectorImage" )
            && ( it->GetDataType() != "Floating_Point_Complex_VectorImage") )
        {
        otbMsgDevMacro( << static_cast<OutputDataDescriptor>(*it));

        // Is it a set of scalar bands ?
        if(  it->GetDataType() == "Labeled_Short_Image"
             || it->GetDataType() == "Labeled_Int_Image"
             || it->GetDataType() == "Labeled_Char_Image"
             || it->GetDataType() == "Floating_Point_Image" )
          {
          // check if it is a band which belongs to the vector image
          if (it->GetDataType().c_str() == prevDataType) // This condition is perhaps too light ?
            {
            new_node_sub = new_node->add_branch(it->GetDataKey().c_str());
            new_node_sub->parent()->open(true);

            new_node_info = new_node_sub->add_branch(m_BranchNameInfo.c_str());
            new_node_info->add(it->GetDataDescription().c_str());
            new_node_info->add(it->GetDataType().c_str());
            new_node_info->branch_icons(&book, &book);

            new_node_sub->branch_icons(&scalarImage, &scalarImage);

            prevDataType.clear();
            prevDataType = it->GetDataType().c_str();
            it++;
            countIt++;
            }
          else
            { // Not a band
            break;
            }
          }
        // Is it a set of complex bands ?
        else if (it->GetDataType() == "Floating_Point_Complex_Image")
          {
          new_node_sub = new_node->add_branch(it->GetDataKey().c_str());
          new_node_sub->parent()->open(true);

          new_node_info = new_node_sub->add_branch(m_BranchNameInfo.c_str());
          new_node_info->add(it->GetDataDescription().c_str());
          new_node_info->add(it->GetDataType().c_str());
          new_node_info->branch_icons(&book, &book);

          it++;
          countIt++;

          // Each complex band generate a set of 4 scalar images so we need to add it.
          FluTreeBrowser::Node* new_node_sub_sub;
          int count = 0;
          while ((count <4) && (it != lDataMap.end()))
            {
            otbMsgDevMacro( << static_cast<OutputDataDescriptor>(*it) );
            new_node_sub_sub = new_node_sub->add_branch(it->GetDataKey().c_str());
            new_node_sub_sub->parent()->open(true);

            new_node_info = new_node_sub_sub->add_branch(m_BranchNameInfo.c_str());
            new_node_info->add(it->GetDataDescription().c_str());
            new_node_info->add(it->GetDataType().c_str());
            new_node_info->branch_icons(&book, &book);

            new_node_sub_sub->branch_icons(&scalarImage, &scalarImage);

            it++;
            countIt++;

            count++;
            }
          }
        else
          {
          // If the current output type is not a VectorImage nor a
          // Image( a Vector_Data for example), the stop condition is
          // never reached. Add a break statement to avoid infinite
          // loop
          break;
          }
        new_node_sub->branch_icons(&scalarImage, &scalarImage);
        }
      new_node->branch_icons(&vectorImage, &vectorImage);
      }

    else if ( it->GetDataType() == "Floating_Point_Image"
            || it->GetDataType() == "Labeled_Short_Image"
            || it->GetDataType() == "Labeled_Int_Image"
            || it->GetDataType() == "Labeled_Char_Image")
      {
      new_node = n->add_branch(it->GetDataKey().c_str());
      new_node->parent()->open(true);

      new_node_info = new_node->add_branch(m_BranchNameInfo.c_str());
      new_node_info->add(it->GetDataDescription().c_str());
      new_node_info->add(it->GetDataType().c_str());
      new_node_info->branch_icons(&book, &book);

      new_node->branch_icons(&scalarImage, &scalarImage);

      it++;
      countIt++;
      }
    else if (it->GetDataType() == "Labeled_Vector_Data"
             || it->GetDataType() == "Vector_Data")
      {
      new_node = n->add_branch(it->GetDataKey().c_str());
      new_node->parent()->open(true);

      new_node_info = new_node->add_branch(m_BranchNameInfo.c_str());
      new_node_info->add(it->GetDataDescription().c_str());
      new_node_info->add(it->GetDataType().c_str());
      new_node_info->branch_icons(&book, &book);

      new_node->branch_icons(&vectorData, &vectorData);

      it++;
      }
    else if (it->GetDataType() == "PleiadesImage")
      {
      new_node = n->add_branch(it->GetDataKey().c_str());
      new_node->parent()->open(true);

      new_node_info = new_node->add_branch(m_BranchNameInfo.c_str());
      new_node_info->add(it->GetDataDescription().c_str());
      new_node_info->add(it->GetDataType().c_str());
      new_node_info->branch_icons(&book, &book);

      new_node->branch_icons(&pleiades, &pleiades);
      it++;
      }

    n->branch_icons(&process, &process);
    //new_node->open(close);
    } // end datas loop
}

void
MonteverdiViewGUI
::ReplaceInTree(const std::string& oldLabel, const std::string& newLabel)
{
  FluTreeBrowser::Node* n  = m_Tree->find(oldLabel.c_str());
  if (n != NULL) n->label(newLabel.c_str());
}

void
MonteverdiViewGUI
::Notify(const MonteverdiEvent& event)
{

  //otbGenericMsgDebugMacro(<< "View: Received event " << event.GetType() << " from module " << event.GetInstanceId());

  // Event received : new instance of module is created
  // -> Open a inputs Window
  if (event.GetType() == "InstanceCreated")
    {
    this->BuildInputsGUI(event.GetInstanceId());
    }
  // event received : module has changed
  else if (event.GetType() == "OutputsUpdated")
    {
    this->UpdateTree(event.GetInstanceId());
    }
  else if (event.GetType() == "Cancel")
    {
//       this->UpdateTree(event.GetInstanceId());
    }
  else if (event.GetType() == "FileDropped")
    {
    this->OpenDroppedFiles();
    }
  else if (event.GetType() == "EventTransfered")
    {
    m_Tree->handle(m_DnDBox->GetTransferedEvent());
    }
  // Event received : UNKNOWN EVENT
  else
    {
    itkExceptionMacro(<< event.GetType() << " is an unknown event.");
    }
}

void
MonteverdiViewGUI
::Show()
{
  wMainWindow->show();
}

void
MonteverdiViewGUI
::Quit()
{
  ModuleMapType::iterator iter;
  ModuleMapType           modMap = m_MonteverdiModel->GetModuleMap();
  // add size>0 because .begin already true -> at least one iteration is done even if size==0
  if (modMap.size() > 0)
    {
    for (iter = modMap.begin(); iter != modMap.end(); iter++)
      {
      (*iter)->Hide();
      }
    }

  gTreeGroup->hide();
  wHelpWindow->hide();
  wMainWindow->hide();

  m_MonteverdiModel->Close();

  if (m_InputViewGUI.IsNotNull())
    {
    m_InputViewGUI->Cancel();
    }

  MsgReporter::GetInstance()->Hide();
}

void MonteverdiViewGUI
::OpenEraseCaching()
{
  if( CachingPathManager::GetInstance()->CachingDirExists() == true )
    {
      wEraseCaching2Window->show();
    }
  else
    {
      this->Quit();
    }
}

void MonteverdiViewGUI
::EraseCaching(bool erase)
{
  wEraseCaching2Window->hide();

  if( erase==true )
    {
      tConfirmErase->value( CachingPathManager::GetInstance()->GetFullCachingPath().c_str() );
      wConfirmCachingWindow->show();
    }
  else
    {
      this->Quit();
    }
}

void MonteverdiViewGUI
::ConfirmEraseCaching( bool erase )
{
  wConfirmCachingWindow->hide();
  m_MonteverdiModel->SetEraseCaching(erase);
 
  this->Quit();
}

void MonteverdiViewGUI
::CancelEraseCaching()
{
  wEraseCaching2Window->hide();
}

void
MonteverdiViewGUI
::Help()
{
  wHelpWindow->show();
}

void
MonteverdiViewGUI
::OpenDroppedFiles()
{
  m_MonteverdiModel->OpenDroppedFiles(m_DnDBox->GetFileNameList());
}


} // end namespace otb
