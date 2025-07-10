/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell_env.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 12:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2025/07/10 15:19:39 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	set_user_env(t_info *info)
{
	char	*user;

	if (!get_env_value(info->env, "USER"))
	{
		user = get_env_value(info->env, "LOGNAME");
		if (user)
			info->env = set_env_var(info->env, "USER", user, 1);
		else
			info->env = set_env_var(info->env, "USER", "user", 1);
	}
}

void	init_shell_env(t_info *info)
{
	info->env = set_env_var(info->env, "?", "0", 1);
	set_user_env(info);
	if (!get_env_value(info->env, "HOME"))
		info->env = set_env_var(info->env, "HOME", "/home/user", 1);
}
