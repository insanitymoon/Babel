#include <common.hpp>





void Combat::Init()
{



}



void Combat::SetPlayerTarget(Player * player, EnemyManager * enemies)
{




	player->SetTarget(NO_TARGET);


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{




		if (glm::distance(player->GetPAttributes()->position, enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->position) < MIN_DISTANCE)
		{


			if (player->GetTarget() == NO_TARGET)
			player->SetTarget(i);



			if (player->GetActionHandler()->GetAction() == ATTACKING)
			{
				player->GetDirection()->Compute(DIR_TYPE_4, player->GetPAttributes()->position,
					enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->target);

			}


			break;



		}


	}




}

  

void Combat::PlayerAttack(SoundManager * sm, Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager *enemies, Map *current_map, int type)
{

	Item *item;
	


	


	if (player->GetTarget() > NO_TARGET && 
		player->GetActionHandler()->GetAction() == ATTACKING && 
		player->GetActionHandler()->HasReachedLifetime(TIME_FOR_ACTION))
	{

		
		



        g_obj->GetTurnSystem()->ComputeAttack(player->GetItems()[ITEM_SLOT_WEAPON]->attack_speed);
		player->GetActionHandler()->SetAction(NO_ACTION);
		player->GetActionHandler()->Stop();



		{

			PhysicalAttributes * e_attr = enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetPAttributes();

			


			GLint hp = enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->hp;
			GLint dmg = enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->Damage(player->GetItems()[ITEM_SLOT_WEAPON]->attack + player->GetStats()->base_attack);


			int rand_item_number;
			if (dmg >= hp)
			{
				player->GetStats()->GetXp()->xp++;

				rand_item_number = enemies->GetEnemiesPointer()[0][player->GetTarget()]->items[Dice::Get((GLuint*)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->chances), (GLuint)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->num_drop), 100)];
			
				if (rand_item_number != EnemyData::item_types::Nothing)
				{

					item = g_obj->GetItemList()->GetList()[rand_item_number];
					g_obj->GetItemList()->AddDroppedItem((int)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetPAttributes()->position.x), (int)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetPAttributes()->position.y), item);
				}
					// std::cout << g_obj->GetItemList()->GetDroppedItems().size()<<"\n";
				enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->Damage(player->GetItems()[ITEM_SLOT_WEAPON]->attack);
				player->GetEventHandler()->Init(current_map, g_obj);
			}

			/*
			
			
			xp = current xp
			max xp = max xp


			*/

			if (player->GetStats()->GetXp()->xp >= player->GetStats()->GetXp()->max_xp)
			{
				int hp = player->GetStats()->GetHp()->hp
					/ player->GetStats()->GetHp()->max_hp;
				int mhp = player->GetStats()->GetXp()->lvl * 2;


				player->GetStats()->GetHp()->Buff(hp * mhp, mhp);
				player->GetStats()->GetXp()->Increase(player->GetStats()->GetXp()->lvl * 4);
				player->GetStats()->base_attack += 2;
				player->GetStats()->GetXp()->lvl++;
				player->GetStats()->GetXp()->xp = 0;
			}

			enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->aggressive = true;


			glm::vec2 text_pos = e_attr->position *
				e_attr->scale +
				g_obj->GetScroller()->GetOffset();

			print_vec2(e_attr->position *
				e_attr->scale);
			print_vec2(text_pos);


			text_pos.y -= 10;
			text_pos.x -= 4;

			char *buffer = new char[256];
			_itoa(dmg, buffer, 10);
			strcat(buffer, " DMG");

			g_obj->GetTextObject()->Add(g_obj->GetFontList()->GetFont(),
				buffer,
				text_pos,
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 20, 
				UP,
				50);



			sm->PlaySound(ATTACK2);


		}
	}




}




void Combat::CheckPlayerMoveAbility(Player * player, EnemyManager * enemies)
{


	GLboolean advance = true;


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		if (glm::distance(player->GetPAttributes()->position, enemies->GetEnemiesPointer()->at(i)->GetPAttributes()->position) < 4.0f)
		{
			advance = false;
			break;
		}

	}


	player->SetMoveAbility(advance);



}




void Combat::PlayerRelated(SoundManager * sm, Controller *ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map, int type)
{


	this->CheckPlayerMoveAbility(player, enemies);
	this->SetPlayerTarget(player, enemies);
	this->PlayerAttack(sm,ctrl, g_obj, player, enemies, map, type);


}




void Combat::SortThingsOut(Player * player, EnemyManager * enemies)
{


	std::vector<Enemy*>*m_enemies = enemies->GetEnemiesPointer();



	for (GLuint i = 0; i < m_enemies->size(); i++)
	{


		for (GLuint j = 0; j < m_enemies->size(); j++)
		{


			if (glm::distance(m_enemies[0][i]->GetPAttributes()->position, player->GetPAttributes()->position) <
				glm::distance(m_enemies[0][j]->GetPAttributes()->position, player->GetPAttributes()->position))
				std::swap(m_enemies[0][i], m_enemies[0][j]);


		}


	}




}



void Combat::SetEnemyTarget(Player * player, EnemyManager * enemies)
{




	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		enemies->GetEnemiesPointer()[0][i]->SetTarget(NO_TARGET);


		if (glm::distance(player->GetPAttributes()->position, enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->position) < MIN_DISTANCE)
			enemies->GetEnemiesPointer()[0][i]->SetTarget(0);



	}


}




void Combat::AquireEnemyTarget(Player * player, EnemyManager * enemies)
{



	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * current_enemy = enemies->GetEnemiesPointer()[0][i];



		if (glm::distance(player->GetPAttributes()->position, current_enemy->GetPAttributes()->position) < MIN_AQUIRE_DISTANCE)
			current_enemy->SetTargetPosition(player->GetPAttributes()->target);
		if (glm::distance(player->GetPAttributes()->position, current_enemy->GetPAttributes()->position) > 2.0f * MIN_AQUIRE_DISTANCE)
			current_enemy->SetTargetPosition(player->GetPAttributes()->target);




	}




}




void Combat::EnemyAttack(SoundManager *sm, Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager *enemies, int type)
{


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * current_enemy = enemies->GetEnemiesPointer()[0][i];


		if (current_enemy->GetTarget() > NO_TARGET && current_enemy->GetActionHandler()->IsStopped())
		{

		
			if (current_enemy->ranged == true)
		    {
				if (current_enemy->GetTurnSystem()->GetTurns() >= 1.0f / current_enemy->GetStats()->base_attack_speed && current_enemy->GetStats()->aggressive == true)
				{

				}
		    }
		
			else if (current_enemy->GetTurnSystem()->GetTurns() >= 1.0f / current_enemy->GetStats()->base_attack_speed && current_enemy->GetStats()->aggressive == true)
				{


					current_enemy->GetDirection()->Compute(DIR_TYPE_4, current_enemy->GetPAttributes()->position, current_enemy->GetTargetPosition());
					current_enemy->GetActionHandler()->SetAction(ATTACKING);
					current_enemy->GetActionHandler()->Start();


				}
			
			


		}



		if (current_enemy->GetActionHandler()->GetAction() == ATTACKING && 
			current_enemy->GetActionHandler()->HasReachedLifetime(TIME_FOR_ACTION))
		{
		


			current_enemy->GetTurnSystem()->ComputeAttack(-current_enemy->GetStats()->base_attack_speed);
			current_enemy->GetActionHandler()->SetAction(NO_ACTION);
			current_enemy->GetActionHandler()->Stop();




			PhysicalAttributes * e_attr = player->GetPAttributes();
			glm::vec2 text_pos = e_attr->position *
				e_attr->scale +
				g_obj->GetScroller()->GetOffset();
			text_pos.y = ctrl->GetWindowHeight() - text_pos.y;// -18;
			text_pos.x -= 4;



			GLint dmg = player->GetStats()->GetHp()->Damage(current_enemy->GetStats()->base_attack);
			char *buffer = new char[256];
			_itoa(dmg, buffer, 10);
			strcat(buffer, " DMG");
			if (type == 0)
				sm->PlaySound(SCORPIONATTACK);
			if (type == 1)
				sm->PlaySound(HYDRAATTACK);
			if (type == 2)
				sm->PlaySound(HYDRAATTACK);


			g_obj->GetTextObject()->Add(g_obj->GetFontList()->GetFont(),
				buffer,
				text_pos,
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 20,
				UP,
				50);


		
		
		}




	}


}





void Combat::EnemyMovement(Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies, Map* current_map, int type)
{




	if (player->GetPAttributes()->HasReachedTarget() &&
		player->GetActionHandler()->GetAction() != ATTACKING &&
		enemies->GetEnemiesPointer()->size() > 0)
		enemies->GetEnemiesPointer()[0][0]->GetTurnLogic()->Advance();



	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * enemy = enemies->GetEnemiesPointer()[0][i];
		PhysicalAttributes * attr = enemy->GetPAttributes();
		TurnSystem * turns = enemy->GetTurnSystem();
		AutoPath * a_path = enemy->GetAutoPath();
		Stats * stats = enemy->GetStats();
		ActionHandler * a_handler = enemy->GetActionHandler();

		EventHandler::DestroyDoor(attr->position, current_map, g_obj);



		if (!enemy->GetTurnLogic()->IsUseless() && enemy->GetStats()->aggressive == true)
		{

			if (attr->position == attr->target &&
				enemy->GetTargetPosition() != vec2_0 &&
				turns->GetTurns() >= 1.0f / stats->base_movement_speed
				&& a_handler->IsStopped())
			{




				a_path->Start(g_obj, enemy->GetPAttributes()->position, enemy->GetTargetPosition(), enemy->GetStats()->path_type);
				a_path->Reset();

		
				int reset_position;
				for (int j = 0; j < g_obj->GetItemList()->GetTraps().size(); j++)
				{
					if (enemy->GetPAttributes()->position == g_obj->GetItemList()->GetTraps()[j]->position && g_obj->GetItemList()->GetTraps()[j]->item->id == CHAINS_ID)
					{
						enemy->GetPAttributes()->Peffects->freeze = true;
						g_obj->GetItemList()->GetTraps()[j]->item->id = CHAINS_ACTIVE_ID;
						reset_position = j;
					}
				}

				
			
				if (a_path->GetPathfinder()->GetPathFound() && enemy->GetPAttributes()->Peffects->freeze == false)
				{


					a_path->SetPath(a_path->GetPathfinder()->GetPath());
					a_path->Advance();




				}
				else
				{
					enemy->GetTurnLogic()->Reset();
					turns->Reset();
					//enemy->GetPAttributes()->Peffects->freeze = false;

				}

			}



			if (a_path->IsSet() && !a_path->FinishedWithoutLast() && turns->GetTurns() >= 1.0f / stats->base_movement_speed)
			{



				attr->target = a_path->GetStep();
				g_obj->GetCollisionMap()->AddToList(glm::ivec2(attr->target));
				g_obj->GetCollisionMap()->GetTiles()[glm::ivec2(attr->target).x][glm::ivec2(attr->target).y] = 1;


				if (a_handler->IsStopped())
				{
					a_handler->Start();
					a_handler->SetAction(MOVING);
				}
	

				if (a_handler->HasReachedLifetime(TIME_FOR_ACTION))
				{
			
					turns->ComputeMovement(-stats->base_movement_speed);
					enemy->GetTurnLogic()->Advance();

					a_handler->Stop();
					a_handler->SetAction(NO_ACTION);
				}



			}


		}
	}


}



void Combat::UpdateTurns(GameObject * g_obj, EnemyManager * enemies)
{


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
		if (enemies->GetEnemiesPointer()[0][i]->GetTargetPosition() != vec2_0)
			g_obj->GetTurnSystem()->Update(enemies->GetEnemiesPointer()[0][i]->GetTurnSystem());

}





void Combat::EnemyRelated(SoundManager *sm, Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map, int type)
{



	this->UpdateTurns(g_obj, enemies);
	this->SortThingsOut(player, enemies);
	this->SetEnemyTarget(player, enemies);
	this->AquireEnemyTarget(player, enemies);
	this->EnemyAttack(sm, ctrl, g_obj, player, enemies, type);
	this->EnemyMovement(ctrl, g_obj, player, enemies,map, type);


}




void Combat::Action(SoundManager * sm, Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map, int type)
{


	
	this->PlayerRelated(sm,ctrl, g_obj, player, enemies, map, type);
	this->EnemyRelated(sm, ctrl,g_obj, player, enemies, map, type);



	g_obj->GetTurnSystem()->Reset();


}






